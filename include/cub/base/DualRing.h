//
// Created by Darwin Yuan on 2021/4/21.
//

#ifndef TRANS_DSL_2_DUALRING_H
#define TRANS_DSL_2_DUALRING_H

#include <cub/cub_ns.h>
#include <cub/base/Placement.h>
#include <cub/base/ValueSize.h>
#include <cub/base/Status.h>
#include <cstdint>

CUB_NS_BEGIN

template <typename T, std::size_t MAX_SIZE>
class DualRing {
    static_assert(MAX_SIZE < std::numeric_limits<uint16_t>::max());
    static constexpr std::size_t SIZE = MAX_SIZE + 1;
    using IndexType = ValueSize_t<SIZE>;
    using OpType = auto (*)(IndexType) -> IndexType;
    using ElemType = Placement<T>;

    template<typename RING, typename ... ARGS>
    auto Push(RING& ring, ARGS&& ... args) -> Status {
        if(auto status = MakeRoom(); status != Result::SUCCESS) {
            return status;
        }

        ring.Push(buff, std::forward<ARGS>(args)...);
        return Result::SUCCESS;
    }

    template<typename RING>
    auto Pop(RING& ring) -> void {
        ring.Pop(buff);
    }

    template<typename RING>
    auto Front(RING const& ring) const -> T const* {
        return ring.Front(buff);
    }

    auto MakeRoom() -> Status {
        if(ring1.tail != ring2.tail) {
            return Result::SUCCESS;
        }

        if(Increase(ring2.head) == ring1.head) {
            return Result::FAILED;
        }

        MoveSpace();

        return Result::SUCCESS;
    }

    auto MoveSpace() -> void {
        if(ring1.GetSize() > ring2.GetSize()) {
            ring2.MoveTo(Decrease(ring1.head), buff);
        } else {
            ring1.MoveTo(Increase(ring2.head), buff);
        }
    }

private:
    static auto Increase(IndexType value) -> IndexType {
        return (value + 1) % SIZE;
    }

    static auto Decrease(IndexType value) -> IndexType {
        return (value + SIZE - 1) % SIZE;
    }

    static auto CalcSize(IndexType end1, IndexType end2) -> IndexType {
        return (end1 + SIZE - end2) % SIZE;
    }

private:
    struct RingBase {
        RingBase(IndexType init) : head{init}, tail{init} {}

        IndexType head;
        IndexType tail;

        auto IsEmpty() const -> bool {
            return head == tail;
        }

        auto Front(ElemType const* buff) const -> T const* {
            return IsEmpty() ? nullptr : buff[head].GetObject();
        }

    protected:
        auto MoveTo(IndexType newHead, ElemType* buff, OpType op) -> void {
            auto newTail = newHead;
            for(auto i = head; i < tail; i = op(i)) {
                buff[newTail].Emplace(*buff[i]);
                buff[i].Destroy();
                newTail = op(newTail);
            }
            head = newHead;
            tail = newTail;
        }

        template<typename ... ARGS>
        auto Push(ElemType* buff, OpType op, ARGS&& ... args) -> void {
            buff[tail].Emplace(std::forward<ARGS>(args)...);
            tail = op(tail);
        }

        auto Pop(ElemType* buff, OpType op) -> void {
            if(IsEmpty()) {
                return;
            }
            buff[head].Destroy();
            head = op(head);
        }
    };

    struct Ring1Policy {
        static constexpr auto Op = Increase;
        static auto GetEnds(RingBase const& ring) -> std::pair<IndexType, IndexType> {
            return {ring.tail, ring.head};
        }
    };

    struct Ring2Policy {
        static constexpr auto Op = Decrease;
        static auto GetEnds(RingBase const& ring) -> std::pair<IndexType, IndexType> {
            return {ring.head, ring.tail};
        }
    };

    template<typename POLICY>
    struct GenericRing : RingBase {
        template<typename ... ARGS>
        auto Push(ElemType* buff, ARGS&& ... args) -> void {
            RingBase::Push(buff, POLICY::Op, std::forward<ARGS>(args)...);
        }

        auto MoveTo(IndexType newHead, ElemType* buff) -> void {
            RingBase::MoveTo(newHead, buff, POLICY::Op);
        }

        auto Pop(ElemType* buff) -> void {
            RingBase::Pop(buff, POLICY::Op);
        }

        auto GetSize() const -> IndexType {
            auto [e1, e2] = POLICY::GetEnds(*this);
            return (e1 + SIZE - e2) % SIZE;
        }
    };

    using Ring1Type = GenericRing<Ring1Policy>;
    using Ring2Type = GenericRing<Ring2Policy>;

    template<typename RING_TYPE>
    struct Ring {
        Ring(RING_TYPE& ring, DualRing& thisObject) : ring(ring), thisObject(thisObject) {}

        template<typename ... ARGS>
        auto Push(ARGS&& ... args) -> Status {
            return thisObject.template Push(ring, std::forward<ARGS>(args)...);
        }

        auto GetSize() const -> IndexType {
            return ring.GetSize();
        }

        auto IsEmpty() const -> bool {
            return ring.IsEmpty();
        }

        auto Pop() -> void {
            thisObject.Pop(ring);
        }

        auto Front() const -> T const* {
            return thisObject.template Front(ring);
        }
    private:
        RING_TYPE& ring;
        DualRing&  thisObject;
    };

public:
    auto Ring1() -> Ring<Ring1Type> {
        return {ring1, *this};
    }

    auto Ring2() -> Ring<Ring2Type> {
        return {ring2, *this};
    }

    auto IsFull() const -> bool {
        return ring1.GetSize() + ring2.GetSize() == MAX_SIZE;
    }

private:
    ElemType buff[SIZE];
    Ring1Type ring1{0};
    Ring2Type ring2{SIZE-1};
};

CUB_NS_END

#endif //TRANS_DSL_2_DUALRING_H
