#pragma once

#include "SinglyLinkedList.h"    
#include "MurmurHash3.h"         
#include <vector>
#include <string>
#include <random>
#include <atomic>

template<typename Key, typename Value>
class ConcurrentHashMap {
private:
    static constexpr size_t MINIMAL_TABLE_SIZE = 10;
    static constexpr size_t DEFAULT_TABLE_SIZE = 100;
    static constexpr double RESIZE_FACTOR = 1.5;
    static constexpr double LOAD_FACTOR = 70.0; 

    struct Bucket {
        SinglyLinkedList<Key, Value> list;
        mutable std::shared_mutex mtx;

        Bucket() : list(), mtx() {}
        Bucket(const Bucket& other) : list(other.list) {}
        Bucket(Bucket&& other) noexcept : list(std::move(other.list)) {}
        Bucket& operator=(const Bucket& other) {
            if (this != &other) {
                list = other.list;
            }
            return *this;
        }
        Bucket& operator=(Bucket&& other) noexcept {
            if (this != &other) {
                list = std::move(other.list);
            }
            return *this;
        }
    };

    std::vector<Bucket> buckets;
    uint32_t seed;
    size_t bucket_count;
    std::atomic<size_t> elements{ 0 };
    mutable std::shared_mutex global_mtx;

    void setBucketCount(size_t count_) {
        std::unique_lock lock(global_mtx);
        if (count_ < MINIMAL_TABLE_SIZE) {
            throw std::invalid_argument("Table size must be at least " + std::to_string(MINIMAL_TABLE_SIZE));
        }
        bucket_count = count_;
    }

    void setSeed() {
        std::random_device rd;
        seed = rd();
    }

    void initialization() {
        std::unique_lock lock(global_mtx);
        buckets.clear();
        buckets.resize(bucket_count);
    }

    size_t hash(const Key& key) const {
        auto hashValues = Hash::MurmurHash3(key, seed);
        size_t combinedHash = static_cast<size_t>(hashValues[0] ^ hashValues[1]);
        return combinedHash % bucket_count;
    }

    size_t hash_with_bucket_count(const Key& key, size_t count) const {
        auto hashValues = Hash::MurmurHash3(key, seed);
        size_t combinedHash = static_cast<size_t>(hashValues[0] ^ hashValues[1]);
        return combinedHash % count;
    }

    void rehash_if_needed() {
        double load = (static_cast<double>(elements.load()) / bucket_count) * 100.0;
        if (load > LOAD_FACTOR) {
            size_t new_bucket_count = static_cast<size_t>(bucket_count * RESIZE_FACTOR);
            resize_internal(new_bucket_count);
        }
    }

    void resize_internal(size_t new_bucket_count) {
        std::unique_lock globalLock(global_mtx);
        std::vector<Bucket> new_buckets;
        new_buckets.resize(new_bucket_count);

        for (size_t i = 0; i < bucket_count; ++i) {
            std::unique_lock bucketLock(buckets[i].mtx);
            size_t bucketSize = buckets[i].list.size();
            for (size_t j = 0; j < bucketSize; ++j) {
                auto element = buckets[i].list[j]; 
                size_t new_index = hash_with_bucket_count(element.first, new_bucket_count);
                {
                    std::unique_lock newBucketLock(new_buckets[new_index].mtx);
                    new_buckets[new_index].list.push_front(element.first, element.second);
                }
            }
        }
        buckets = std::move(new_buckets);
        bucket_count = new_bucket_count;
    }

public:
    explicit ConcurrentHashMap() : ConcurrentHashMap(DEFAULT_TABLE_SIZE) {}

    explicit ConcurrentHashMap(size_t table_size) {
        setBucketCount(table_size);
        initialization();
        setSeed();
    }

    ConcurrentHashMap(const ConcurrentHashMap& other) {
        std::shared_lock globalLock(other.global_mtx);
        bucket_count = other.bucket_count;
        seed = other.seed;
        elements = other.elements.load();
        buckets.resize(bucket_count);
        for (size_t i = 0; i < bucket_count; ++i) {
            std::shared_lock bucketLock(other.buckets[i].mtx);
            buckets[i] = other.buckets[i];
        }
    }

    ConcurrentHashMap(ConcurrentHashMap&& other) noexcept {
        std::unique_lock globalLock(other.global_mtx);
        bucket_count = other.bucket_count;
        seed = other.seed;
        elements = other.elements.load();
        buckets = std::move(other.buckets);
        other.bucket_count = 0;
        other.elements = 0;
    }

    ConcurrentHashMap& operator=(const ConcurrentHashMap& other) {
        if (this != &other) {
            std::unique_lock globalLock1(global_mtx, std::defer_lock);
            std::shared_lock globalLock2(other.global_mtx, std::defer_lock);
            std::lock(globalLock1, globalLock2);

            bucket_count = other.bucket_count;
            seed = other.seed;
            elements = other.elements.load();
            buckets.resize(bucket_count);
            for (size_t i = 0; i < bucket_count; ++i) {
                std::shared_lock bucketLock(other.buckets[i].mtx);
                buckets[i] = other.buckets[i];
            }
        }
        return *this;
    }

    ConcurrentHashMap& operator=(ConcurrentHashMap&& other) noexcept {
        if (this != &other) {
            std::unique_lock globalLock1(global_mtx, std::defer_lock);
            std::unique_lock globalLock2(other.global_mtx, std::defer_lock);
            std::lock(globalLock1, globalLock2);

            bucket_count = other.bucket_count;
            seed = other.seed;
            elements = other.elements.load();
            buckets = std::move(other.buckets);
            other.bucket_count = 0;
            other.elements = 0;
        }
        return *this;
    }

    ~ConcurrentHashMap() {
        clear();
    }

    void insert(const Key& key, const Value& value) {
        size_t index = hash(key);
        {
            std::unique_lock bucketLock(buckets[index].mtx);
            buckets[index].list.push_front(key, value);
        }
        elements.fetch_add(1);
        rehash_if_needed();
    }

    std::optional<Value> remove(const Key& key) {
        size_t index = hash(key);
        std::unique_lock bucketLock(buckets[index].mtx);
        auto indexOpt = buckets[index].list.find_index_by_key(key);
        if (indexOpt.has_value()) {
            Value value = buckets[index].list[indexOpt.value()].second;
            buckets[index].list.erase(indexOpt.value());
            elements.fetch_sub(1);
            return value;
        }
        return std::nullopt;
    }

    std::optional<Value> search(const Key& key) const {
        size_t index = hash(key);
        std::shared_lock bucketLock(buckets[index].mtx);
        auto indexOpt = buckets[index].list.find_index_by_key(key);
        if (indexOpt.has_value()) {
            return buckets[index].list[indexOpt.value()].second;
        }
        return std::nullopt;
    }

    void clear() {
        std::unique_lock globalLock(global_mtx);
        for (auto& bucket : buckets) {
            std::unique_lock bucketLock(bucket.mtx);
            bucket.list.clear();
        }
        elements.store(0);
    }

    void resize(size_t new_size) {
        std::unique_lock globalLock(global_mtx);
        resize_internal(new_size);
    }

    double loadFactor() const {
        return (static_cast<double>(elements.load()) / bucket_count) * 100.0;
    }

    size_t getElementsCount() const {
        return elements.load();
    }

    size_t getBucketCount() const {
        return bucket_count;
    }
};
