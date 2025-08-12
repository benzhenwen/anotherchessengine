#pragma once

#include <lib/chess/util.hpp>
#include <lib/chess/move.hpp>

namespace Chess::Engine {
class TranspositionTable {

public:
    struct Node {
        U32 partial_hash; // 32 bits / 4B
        Move best_move;   // 16 bits / 2B

        I16 score;        // 16 bits / 2B
        U8 depth;         // 8 bits  / 1B

        enum Type: U8 {EXACT, UPPERBOUND, LOWERBOUND} type; // 8 bits / 1B
        U8 generation; // 8 bits / 1B

    private:
        U16 padding;

        // total = 16B

    public:
        constexpr Node():
            partial_hash(0),
            best_move(0),
            score(0),
            depth(0),
            type(EXACT),
            generation(0),
            padding()
        {}
        constexpr Node(U32 _partial_hash, Move _best_move, I16 _score, U8 _depth, Type _type, I8 _generation):
            partial_hash(_partial_hash),
            best_move(_best_move),
            score(_score),
            depth(_depth),
            type(_type),
            generation(_generation),
            padding()
        {}
    };
    static_assert(sizeof(Node) == 16);

private:
    static constexpr size_t TT_MB = 8; // MB
    static constexpr size_t BUCKET_SIZE = 64; // bytes per bucket
    static constexpr size_t BUCKETS = (TT_MB * 1048576) / BUCKET_SIZE; // 16777216

    static_assert((BUCKETS & (BUCKETS-1))==0);

    size_t buckets_cnt;
    std::vector<Node> table; // [total buckets][bucket idx] -> each bucket is 64 bytes

    U8 curr_generation = 1;

    inline Node* bucket_ptr(uint64_t full_hash) {
        size_t idx = (size_t(full_hash % buckets_cnt)) * 4;
        return & table[idx];
    }

public:
    explicit TranspositionTable(size_t mb):
        buckets_cnt((mb * 1048576) / 64),
        table(buckets_cnt * 4) {}

    inline void bump_generation() {
        curr_generation++;
    }

public:
    inline void add_entry(U64 full_hash, const Move & best_move, I16 score, U8 depth, Node::Type type) {
        Node * bucket = bucket_ptr(full_hash);
        const U32 node_partial_hash = full_hash >> 32;

        // bucket selection
        // replace same bucket
        int same = -1;
        for (int i = 0; i < 4; i++) {
            if(bucket[i].generation == curr_generation && bucket[i].partial_hash == node_partial_hash) {
                same = i;
                break;
            }
        }
        int victim = same;

        // pick stale / empty bucket
        if (victim == -1) {
            for (int i = 0; i < 4; i++) {
                if (bucket[i].generation != curr_generation) {
                    victim = i;
                    break;
                }
            }
        }

        // pick shallowest, then oldest generation
        if (victim == -1) {
            int shallow = 0;
            for (int i = 0; i < 4; i++) {
                if (bucket[i].depth < bucket[shallow].depth || // bucket has a higher depth
                    (bucket[i].depth == bucket[shallow].depth && ((curr_generation - bucket[i].generation) > (curr_generation - bucket[shallow].generation)))) { // bucket tied depth, older generation
                        shallow = i;
                }
            }
            victim = shallow;
        }

        Node & n = bucket[victim];
        n.partial_hash = node_partial_hash;
        n.best_move = best_move.v;
        n.score = score;
        n.depth = depth;
        n.type = type;
        n.generation = curr_generation;
    }   


    struct EntryData {
        Move best_move = Move();
        I16 score = 0;
        U8 depth = 0;
        Node::Type type = Node::EXACT;

        constexpr bool is_valid() {
            return best_move.v != 0;
        }
    };
    inline EntryData find_entry(U64 full_hash) {
        Node* bucket = bucket_ptr(full_hash);
        const U32 node_partial_hash = full_hash >> 32;

        // find entry
        for (int i = 0; i < 4; i++) {
            if (bucket[i].partial_hash == node_partial_hash) {
                return {
                    bucket[i].best_move,
                    bucket[i].score,
                    bucket[i].depth,
                    bucket[i].type
                };
            }
        }

        return EntryData{0, 0, 0, Node::EXACT}; // not found
    }

};
}