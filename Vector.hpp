#ifndef __CUSTOM_VECTOR__
#define __CUSTOM_VECTOR__

#include <iterator>
#include <type_traits>
#include <memory>

namespace custom
{
    ////////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename U>
    struct is_same
    {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_same<T, T>
    {
        static constexpr bool value = true;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    void advance(Iterator& it, int n)
    {
        if constexpr (is_same<typename std::iterator_traits<Iterator>::iterator_category,
                std::random_access_iterator_tag>::value)
        {
            it += n;
        }
        else
        {
            if (n >= 0) for (int i = 0; i < n; ++i, ++it);
            else for (int i = n; i > 0; --i, --it);
        }
    }

    template <bool B, typename T, typename F>
    struct conditional
    {
        using type = F;
    };

    template <typename T, typename F>
    struct conditional<true, T, F>
    {
        using type = T;
    };

    template <bool B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;

    ////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct StandartAllocator
    {
        T* allocate(size_t n) const;
        void deallocate(T* ptr, size_t) const;

        template <typename... Args>
        void construct(T* ptr, const Args&... args) const;

        void destroy(T* ptr) const;
    };    

    ////////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename Alloc = StandartAllocator<T>>
    struct VectorBase
    {
        Alloc m_alloc;
        T* m_start;
        T* m_end;
        T* m_spaceEnd;

        using AllocTraits = std::allocator_traits<Alloc>;

        VectorBase(const Alloc alloc, std::size_t n)
            : m_alloc(alloc), m_start(AllocTraits::allocate(m_alloc, n)),
              m_end(m_start), m_spaceEnd(m_start + n)
        {}

        void free_memory() 
        { AllocTraits::deallocate(m_alloc, m_start, m_spaceEnd - m_start); }

        ~VectorBase()
        { free_memory(); }
    };

    template <typename T, typename Alloc>
    void swap(VectorBase<T, Alloc>& a, VectorBase<T, Alloc>& b)
    {
        std::swap(a.m_alloc, b.m_alloc);
        std::swap(a.m_start, b.m_start);
        std::swap(a.m_end, b.m_end);
        std::swap(a.m_spaceEnd, b.m_spaceEnd);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename Alloc = StandartAllocator<T>>
    class Vector : private VectorBase<T, Alloc>
    {
        using VectorBase<T, Alloc>::m_alloc;
        using VectorBase<T, Alloc>::m_start;
        using VectorBase<T, Alloc>::m_end;
        using VectorBase<T, Alloc>::m_spaceEnd;
        using AllocTraits = typename VectorBase<T, Alloc>::AllocTraits;

        void destroy_elements();

    public:
        template <bool IsConst>
        class common_iterator
        {
        private:
            conditional_t<IsConst, const T*, T*> m_ptr = nullptr;

        public:
            common_iterator(T* ptr);

            conditional_t<IsConst, const T&, T&> operator * () const;
            conditional_t<IsConst, const T*, T*> operator -> () const;
            common_iterator<IsConst>& operator ++ ();
            common_iterator<IsConst>& operator -- ();
            common_iterator<IsConst>& operator += (std::size_t n);
            common_iterator<IsConst>& operator -= (std::size_t n);
            common_iterator<IsConst> operator ++ (int);
            common_iterator<IsConst> operator -- (int);
            common_iterator<IsConst> operator - (std::size_t n);
            common_iterator<IsConst> operator + (std::size_t n);
        };

        using iterator = common_iterator<false>;
        using const_iterator = common_iterator<true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Vector(const Alloc& alloc = Alloc());

        Vector(std::size_t n, const T& value = T(),
               const Alloc& alloc = Alloc());

        Vector(const Vector<T, Alloc>& vec);
        Vector(Vector<T, Alloc>&& vec); //

        Vector<T, Alloc>& operator = (const Vector<T, Alloc>& vec); //
        Vector<T, Alloc>& operator = (Vector<T, Alloc>&& vec); //
        void safe_assign(const Vector<T, Alloc>& vec); //

        std::size_t size() const;
        std::size_t capacity() const;
        const Alloc& get_allocator() const; //

        void resize(std::size_t n, T value = T());
        void reserve(std::size_t n);
        void shrink_to_fit(); //
        void clear(); //

        void push_back(T value = T());
        iterator insert(const_iterator pos, const T& value); //
        iterator insert(const_iterator pos, T&& value); //

        template <typename... Args>
        iterator emplace(const_iterator pos, Args&&... args); //

        template <typename... Args>
        T& emplace_back(Args&&... args); //

        iterator erase(const_iterator pos); //
        void pop_back();

        T* data();
        T const * data() const;

        T& front();
        const T& front() const;
        T& back();
        const T& back() const;
        T& operator [] (std::size_t i);
        const T& operator [] (std::size_t i) const;
        T& at(std::size_t i);
        const T& at(std::size_t i) const;

        iterator begin() const;
        iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        reverse_iterator rbegin() const;
        reverse_iterator rend() const;
        const_reverse_iterator rcbegin() const;
        const_reverse_iterator rcend() const;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    //template <typename Alloc = StandartAllocator<int8_t>>
    //class BitVector //
    //{
    //private:
    //    Alloc m_alloc;
    //    int8_t* m_arr = nullptr;
    //    std::size_t m_size = 0;
    //    std::size_t m_capacity = 0;

    //    using AllocTraits = std::allocator_traits<Alloc>;

    //public:
    //    class BitReference
    //    {
    //    private:
    //        int8_t* m_cell;
    //        int8_t m_num;

    //    public:
    //        BitReference(int8_t* cell, int8_t num);
    //        BitReference& operator = (bool b);
    //        operator bool () const;
    //    };

    //    using ConstBitReference = bool;

    //    template <bool IsConst>
    //    class common_iterator
    //    {
    //    private:
    //        conditional_t<IsConst, ConstBitReference, BitReference> m_ref;

    //    public:
    //        common_iterator(int8_t* cell, int8_t num);

    //        conditional_t<IsConst, ConstBitReference, BitReference> operator * () const;
    //        conditional_t<IsConst, ConstBitReference, BitReference> operator -> () const;
    //        common_iterator<IsConst>& operator ++ ();
    //    };

    //public:
    //    using iterator = common_iterator<false>;
    //    using const_iterator = common_iterator<true>;
    //    using reverse_iterator = std::reverse_iterator<iterator>;
    //    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //public: 
    //    BitVector();

    //    BitVector(std::size_t n, bool value = bool(),
    //              const Alloc& alloc = Alloc());

    //    BitVector(const BitVector<Alloc>& vec);
    //    BitVector(BitVector<Alloc>&& vec);

    //    std::size_t size() const;
    //    std::size_t capacity() const;
    //    Alloc get_allocator() const; //

    //    void resize(std::size_t n, bool value = bool());
    //    void reserve(std::size_t n);
    //    void shrink_to_fit(); //
    //    void clear(); //

    //    void push_back(bool value = bool());
    //    iterator insert(const_iterator pos, bool value); //
    //    iterator emplace(const_iterator pos, bool value); //

    //    BitReference emplace_back(bool value); //

    //    iterator erese(const_iterator pos); //
    //    void pop_back(); //

    //    int8_t * data(); //
    //    int8_t const * data() const; //

    //    BitReference front(); //
    //    ConstBitReference front() const; //
    //    BitReference back(); //
    //    ConstBitReference back() const; //
    //    BitReference operator [] (std::size_t i);
    //    ConstBitReference operator [] (std::size_t i) const;
    //    BitReference at(std::size_t i); //
    //    ConstBitReference at(std::size_t i) const; //

    //    iterator begin() const; //
    //    iterator end() const; //
    //    const_iterator cbegin() const; //
    //    const_iterator cend() const; //
    //    reverse_iterator rbegin() const; //
    //    reverse_iterator rend() const; //
    //    const_reverse_iterator rcbegin() const; //
    //    const_reverse_iterator rcend() const; //
    //};

    ////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------------------- //
    template <typename T>
    T* StandartAllocator<T>::allocate(size_t n) const
    {
        return ::operator new(n * sizeof(T));
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T>
    void StandartAllocator<T>::deallocate(T* ptr, size_t) const
    {
        operator delete(ptr);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T>
    template <typename... Args>
    void StandartAllocator<T>::construct(T* ptr, const Args&... args) const
    {
        new (ptr) T(args...);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T>
    void StandartAllocator<T>::destroy(T* ptr) const
    {
        ptr->~T();
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    Vector<T, Alloc>::common_iterator<IsConst>::common_iterator(T* ptr)
        : m_ptr(ptr)
    {}

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    conditional_t<IsConst, const T&, T&>
    Vector<T, Alloc>::common_iterator<IsConst>::operator * () const
    {
        return *m_ptr;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    conditional_t<IsConst, const T*, T*>
    Vector<T, Alloc>::common_iterator<IsConst>::operator -> () const
    {
        return m_ptr;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>&
    Vector<T, Alloc>::common_iterator<IsConst>::operator ++ ()
    {
        ++m_ptr;
        return *this;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>
    Vector<T, Alloc>::common_iterator<IsConst>::operator ++ (int)
    {
        return common_iterator<IsConst>(m_ptr++);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>&
    Vector<T, Alloc>::common_iterator<IsConst>::operator -- ()
    {
        --m_ptr;
        return *this;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>
    Vector<T, Alloc>::common_iterator<IsConst>::operator -- (int)
    {
        return common_iterator<IsConst>(m_ptr--);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>&
    Vector<T, Alloc>::common_iterator<IsConst>::operator += (std::size_t n)
    {
        m_ptr += n;
        return *this;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>&
    Vector<T, Alloc>::common_iterator<IsConst>::operator -= (std::size_t n)
    {
        m_ptr -= n;
        return *this;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>
    Vector<T, Alloc>::common_iterator<IsConst>::operator - (std::size_t n)
    {
        auto copy(*this);
        return copy += n;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    template <bool IsConst>
    typename Vector<T, Alloc>::template common_iterator<IsConst>
    Vector<T, Alloc>::common_iterator<IsConst>::operator + (std::size_t n)
    {
        auto copy(*this);
        return copy -= n;
    }

    
    ////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    void Vector<T, Alloc>::destroy_elements()
    {
        for (T* p = m_start; p != m_end; ++p) AllocTraits::destroy(m_alloc, p);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    Vector<T, Alloc>::Vector(const Alloc& alloc)
        : VectorBase<T, Alloc>(alloc, 0)
    {}

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    Vector<T, Alloc>::Vector(size_t n, const T& value, const Alloc& alloc)
        : VectorBase<T, Alloc>(alloc, n)
    {
        std::uninitialized_fill(m_start, m_spaceEnd, value);
        m_end = m_start + n;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    Vector<T, Alloc>::Vector(const Vector<T, Alloc>& vec)
        : VectorBase<T, Alloc>(vec.alloc, vec.capacity())
    {
        std::uninitialized_copy(vec.m_start, vec.m_end, m_start);
        m_end = m_start + vec.size();
    }

        //T* newarr = AllocTraits::allocate(m_alloc, vec.m_capacity);
        //
        //std::size_t i = 0;
        //try
        //{
        //    for (; i < vec.m_size; ++i)
        //        AllocTraits::construct(m_alloc, newarr + i, vec.m_arr[i]);
        //}
        //catch (...)
        //{
        //    for (std::size_t k = 0; k < i; ++k)
        //        AllocTraits::destroy(m_alloc, newarr + k);
        //    AllocTraits::deallocate(m_alloc, newarr, vec.m_capacity);
        //    throw;
        //}

        //for (std::size_t k = 0; k < m_size; ++k)
        //    AllocTraits::destroy(m_alloc, m_arr + k);
        //AllocTraits::deallocate(m_alloc, m_arr, m_size);

        //m_arr = newarr;
        //m_capacity = vec.m_capacity;
        //m_size = vec.m_size;
    //}

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    Vector<T, Alloc>::Vector(Vector<T, Alloc>&& vec) {}

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    Vector<T, Alloc>& Vector<T, Alloc>::operator = (const Vector<T, Alloc>& vec)
    {
        if (AllocTraits::propagate_on_container_copy_assignment::value
            || (capacity() < vec.size()))
        {
            safe_assign(vec);
            return *this;
        }
        else
        {
            if (this == &vec) return *this; 

            std::size_t sz = size();
            std::size_t vecSz = vec.size();

            if (vecSz <= sz)
            {
                std::copy(vec.m_start, vec.m_end, m_start);
                for (T* p = m_start + vecSz; p < m_end; ++p)
                    AllocTraits::destroy(m_alloc, p);
            }
            else
            {
                std::copy(vec.m_start, vec.m_start + sz, m_start);
                std::uninitialized_copy(vec.m_start + sz, vec.m_end, m_end);
            }
            m_end = m_start + vecSz;
            return *this;
        }
    }

    //template <typename T, typename Alloc>
    //Vector<T, Alloc>& operator = (Vector<T, Alloc>&& vec); //

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    void Vector<T, Alloc>::safe_assign(const Vector<T, Alloc>& vec)
    {
        if (this == &vec) return;

        if (AllocTraits::propagate_on_container_copy_assignment::value
            && m_alloc != vec.m_alloc)
        {
            Vector<T, Alloc> temp(vec);
            swap<VectorBase<T, Alloc>>(temp, *this);
        }
        else
        {
            T* newStart = AllocTraits::allocate(m_alloc, vec.capacity());
            std::uninitialized_copy(m_start, m_end, newStart);
            
            destroy_elements();
            VectorBase<T, Alloc>::free_memory();

            m_start = newStart;
            m_end = m_start + vec.size();
            m_spaceEnd = m_start + vec.capacity();
        }
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    std::size_t Vector<T, Alloc>::size() const
    {
        return m_end - m_start;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    std::size_t Vector<T, Alloc>::capacity() const
    {
        return m_spaceEnd - m_start;
    }
    
    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    void Vector<T, Alloc>::resize(std::size_t n, T value)
    {
        if (n > capacity()) reserve(n);

        if (n > size())
        {
            while (size() < n) push_back(value);            
        }
        else
        {
            while (size() > n) pop_back();
        }
    }
    
    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    void Vector<T, Alloc>::reserve(std::size_t n)
    {
        if (n <= capacity()) return;

        VectorBase<T, Alloc> temp(m_alloc, size() ? size() * 2 : 2);

        std::uninitialized_copy(m_start, m_end, temp.m_start);
        temp.m_end = temp.m_start + size();
        destroy_elements();
        swap<VectorBase<T, Alloc>>(temp, *this);
    }
        
        //T* newarr = AllocTraits::allocate(m_alloc, n);

        //std::size_t i = 0;
        //try
        //{
        //    for (; i < m_size; ++i)
        //        AllocTraits::construct(m_alloc, newarr + i, m_arr[i]);
        //}
        //catch(...)
        //{
        //    for (std::size_t k = 0; k < i; ++k)
        //        AllocTraits::destroy(m_alloc, newarr + k);
        //    AllocTraits::deallocate(m_alloc, newarr, n);
        //    throw;
        //}

        //for (std::size_t k = 0; k < m_size; ++k)
        //    AllocTraits::destroy(m_alloc, m_arr + k);
        //AllocTraits::deallocate(m_alloc, m_arr, m_size);

        //m_arr = newarr;
        //m_capacity = n;
    //}

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    void Vector<T, Alloc>::push_back(T value)
    {
        if (m_end == m_spaceEnd)
        {
            VectorBase<T, Alloc> temp(m_alloc, size() ? size() * 2 : 2);
            std::uninitialized_copy(m_start, m_end, temp.m_start);
            temp.m_end = temp.m_start + size();

            AllocTraits::construct(temp.m_alloc, temp.m_end, value);
            ++temp.m_end;

            destroy_elements();
            swap<VectorBase<T, Alloc>>(temp, *this);
        }

        AllocTraits::construct(m_alloc, m_end,value);
        ++m_end;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    void Vector<T, Alloc>::pop_back()
    {
        AllocTraits::destroy(m_alloc, m_end);
        --m_end;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    T* Vector<T, Alloc>::data()
    {
        return m_start;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    T const * Vector<T, Alloc>::data() const
    {
        return m_start;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    T& Vector<T, Alloc>::front()
    {
        return *m_start;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    const T& Vector<T, Alloc>::front() const
    {
        return *m_start;
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    T& Vector<T, Alloc>::back()
    {
        return *(m_end - 1);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    const T& Vector<T, Alloc>::back() const
    {
        return *(m_end - 1);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    T& Vector<T, Alloc>::operator [] (std::size_t i)
    {
        return *(m_start + i);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    const T& Vector<T, Alloc>::operator [] (std::size_t i) const
    {
        return *(m_start + i);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    T& Vector<T, Alloc>::at(std::size_t i)
    {
        if (i >= size())
            throw std::out_of_range("Index out of range");

        return *(m_start + i);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    const T& Vector<T, Alloc>::at(std::size_t i) const
    {
        if (i >= size())
            throw std::out_of_range("Index out of range");

        return *(m_start + i);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::iterator
    Vector<T, Alloc>::begin() const
    {
        return iterator(m_start);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::iterator
    Vector<T, Alloc>::end() const
    {
        return iterator(m_end);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::const_iterator
    Vector<T, Alloc>::cbegin() const
    {
        return const_iterator(m_start);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::const_iterator
    Vector<T, Alloc>::cend() const
    {
        return const_iterator(m_end);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::reverse_iterator
    Vector<T, Alloc>::rbegin() const
    {
        return reverse_iterator(m_end - 1);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::reverse_iterator
    Vector<T, Alloc>::rend() const
    {
        return reverse_iterator(m_start - 1);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::const_reverse_iterator
    Vector<T, Alloc>::rcbegin() const
    {
        return const_reverse_iterator(m_end - 1);
    }

    // ---------------------------------------------------------------------------------- //
    template <typename T, typename Alloc>
    typename Vector<T, Alloc>::const_reverse_iterator
    Vector<T, Alloc>::rcend() const
    {
        return const_reverse_iterator(m_start - 1);
    }
};

#endif // __CUSTOM_VECTOR__
