#pragma once

#include <iterator>
#include <initializer_list>
#include <utility>
#include <cassert>

template <typename Type>
class SingleLinkedList{
private:
    struct Node{
        Node() = default;
        Node(const Type& val, Node* next) : value(val), next_node(next) {}

        Node(Type&& val, Node* next) : value(std::move(val)), next_node(next) {}

        Type value{};
        Node* next_node = nullptr;
    };

    Node head_;
    size_t size_ = 0;

    // Inner class iterator
    template <typename ValueType>
    class BasicIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] operator bool() const noexcept {
            return node_ != nullptr;
        }

        BasicIterator& operator++() noexcept {
            assert(node_);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            assert(node_);
            BasicIterator old_iter = *this;
            node_ = node_->next_node;
            return old_iter;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_);
            return &(node_->value);
        }
    private:
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_{node} {}

        Node* node_ = nullptr;
    };

public:

    // Methods related to the iterators:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&this->head_)};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    // Constructors and destructor:
    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> list) {
        Initialize<std::initializer_list<Type>>(list);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        Initialize<SingleLinkedList<Type>>(other);
    }

    SingleLinkedList& operator=(const SingleLinkedList& other) {
        if(this != &other) {
            Initialize<SingleLinkedList<Type>>(other);
        }
        return *this;
    }

    SingleLinkedList(SingleLinkedList&& other) {
        swap(other);
    }

    SingleLinkedList& operator=(SingleLinkedList&& other) {
        if(this != &other) {
            this = &other;
        }
        return *this;
    }

    ~SingleLinkedList() {
        Clear();
    }

    // Different methods:
    void Clear() noexcept {
        while(head_.next_node) {
            Node* first = head_.next_node;
            head_.next_node = first->next_node;
            delete first;
        }
        size_ = 0;
    }

    int GetSize() const noexcept {
        return size_;
    }

    bool IsEmpty() const noexcept {
        return head_.next_node == nullptr;
    }

    // Add an item to the front of the list
    template <typename T>
    void PushFront(T&& value) {
        Node* new_node = new Node(std::forward<T>(value), head_.next_node);
        head_.next_node = new_node;
        ++size_;
    }

    template <typename T>
    Iterator InsertAfter(ConstIterator pos, T&& value) {
        // ИЗМЕНЕНО: используем assert
        assert(pos);
        Node* insert_node = new Node(std::forward<T>(value), pos.node_->next_node);
        pos.node_->next_node = insert_node;

        ++size_;

        return Iterator{insert_node};
    }
    
     void PopFront() noexcept {
        // ИЗМЕНЕНО: assert 
        assert(!IsEmpty());
        if (IsEmpty()) // дополнительная защита в release (чтобы не UB)
            return;

        Node* pop_node = head_.next_node;
        head_.next_node = pop_node->next_node;

        delete pop_node;
        --size_;
    }

   Iterator EraseAfter(ConstIterator pos) noexcept {
        // ИЗМЕНЕНО: assert 
        assert(pos && pos.node_->next_node);
        if (!pos || !pos.node_->next_node) // дополнительная защита в release
            return Iterator{nullptr};

        Node* erase_node = pos.node_->next_node;
        pos.node_->next_node = erase_node->next_node;

        delete erase_node;
        --size_;

        return Iterator{pos.node_->next_node};
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

private:
    template <typename Container>
    void Initialize(const Container& container) {
        SingleLinkedList<Type> temp;

        Node* node = &temp.head_;

        for(auto& element : container) {
            node->next_node = new Node(element, nullptr);
            node = node->next_node;
            ++temp.size_;
        }

        swap(temp);
    }
};

template <typename Type>
void swap(SingleLinkedList<Type>& left, SingleLinkedList<Type>& right) noexcept {
    left.swap(right);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
    if (&left == &right)
        return true;
    if (left.GetSize() != right.GetSize())
        return false;
    return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
    return !(left == right);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
    return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
    return !(right < left);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
    return right < left;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
    return !(left < right);
}
