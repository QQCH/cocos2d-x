/****************************************************************************
Copyright (c) 2010 ForzeField Studios S.L. http://forzefield.com
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef __CCVECTOR_H__
#define __CCVECTOR_H__

#include "base/ccMacros.h"
#include "base/CCRef.h"
#include <vector>
#include <functional>
#include <algorithm> // for std::find

NS_CC_BEGIN

template<class T>
class CC_DLL Vector
{
public:
    // ------------------------------------------
    // Iterators
    // ------------------------------------------
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    
    typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
    
    iterator begin() { return _data.begin(); }
    const_iterator begin() const { return _data.begin(); }
    
    iterator end() { return _data.end(); }
    const_iterator end() const { return _data.end(); }
    
    const_iterator cbegin() const { return _data.cbegin(); }
    const_iterator cend() const { return _data.cend(); }
    
    reverse_iterator rbegin() { return _data.rbegin(); }
    const_reverse_iterator rbegin() const { return _data.rbegin(); }
    
    reverse_iterator rend() { return _data.rend(); }
    const_reverse_iterator rend() const { return _data.rend(); }
    
    const_reverse_iterator crbegin() const { return _data.crbegin(); }
    const_reverse_iterator crend() const { return _data.crend(); }
    
    /** ���캯�� */
    Vector<T>()
    : _data()
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
    }
    
    /** �����������Ĺ��캯�� */
    explicit Vector<T>(ssize_t capacity)
    : _data()
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
        CCLOGINFO("In the default constructor with capacity of Vector.");
        reserve(capacity);
    }

    /** �������� */
    ~Vector<T>()
    {
        CCLOGINFO("In the destructor of Vector.");
        clear();
    }

    /** �������캯�� */
    Vector<T>(const Vector<T>& other)
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
        CCLOGINFO("In the copy constructor!");
        _data = other._data;
        addRefForAllObjects();
    }
    
    /** ת�ƹ��캯�� */
    Vector<T>(Vector<T>&& other)
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
        CCLOGINFO("In the move constructor of Vector!");
        _data = std::move(other._data);
    }
    
    /** ������ֵ������ */
    Vector<T>& operator=(const Vector<T>& other)
    {
        if (this != &other) {
            CCLOGINFO("In the copy assignment operator!");
            clear();
            _data = other._data;
            addRefForAllObjects();
        }
        return *this;
    }
    
    /** ת�Ƹ�ֵ������ */
    Vector<T>& operator=(Vector<T>&& other)
    {
        if (this != &other) {
            CCLOGINFO("In the move assignment operator!");
            clear();
            _data = std::move(other._data);
        }
        return *this;
    }
    
// Don't uses operator since we could not decide whether it needs 'retain'/'release'.
//    T& operator[](int index)
//    {
//        return _data[index];
//    }
//    
//    const T& operator[](int index) const
//    {
//        return _data[index];
//    }
    
	/** @brief ����ı�����(vector)������ 
     *  @param capacity ����(vector)����С����.
     *         ���n�ȵ�ǰ����(vector)��������, 
     *         ������������·�������(vector)�Ĵ洢�ռ䣬������������n(���߸���).
     */
    void reserve(ssize_t n)
    {
        _data.reserve(n);
    }
    
	 /** @brief ���ص�ǰ����������Ĵ洢�ռ�Ĵ�С�����������е�Ԫ�ظ�������ʾ.
     *  @note �������û��Ҫ������(vector)��size���. 
     *        ��������Ȼ��߸����������Ļ����ͻ��ж���Ŀռ�����Ӧ�������������Ͳ���Ҫ��ÿ�β������ʱ�����·���ռ�
     *  @return ��ǰ���������(vector)�Ĵ洢�����Ĵ�С�������С�ǰ������ܹ���ŵ�Ԫ�صĸ��������
     */
    ssize_t capacity() const
    {
        return _data.capacity();
    }
    
	 /** @brief ��������(vector)��Ԫ�ص���Ŀ.
     *  @note ��������(vector)�д洢��ʵ�ʶ������Ŀ, ����Ҫ�����Ĵ洢�������.
     *  @return ������Ԫ�ص�ʵ����Ŀ.
     */
    ssize_t size() const
    {
        return  _data.size();
    }

	 /** @brief ��������(vector)�Ƿ��ǿյ� (���磬����size�Ƿ���0).
     *  @note ��������������κη�ʽ�ı�������������������(vector)�����ݣ���ο� Vector<T>::clear
     */
    bool empty() const
    {
        return _data.empty();
    }
    
    /** ��������(vector)�������ɵ����Ԫ����Ŀ. */
    ssize_t max_size() const
    {
        return _data.max_size();
    }
    
    /** �����ض����������, �������(vector)�в������˶����򷵻�UINT_MAX */
    ssize_t getIndex(T object) const
    {
        auto iter = std::find(_data.begin(), _data.end(), object);
        if (iter != _data.end())
            return iter - _data.begin();

        return -1;
    }

    /** @brief Find the object in the vector.
     *  @return Returns an iterator to the first element in the range [first,last) that compares equal to val. 
     *          If no such element is found, the function returns last.
     */
	 /** @brief ������(vector)�в��Ҷ���.
     *  @return ����һ��ָ������[first, last)�еĵ�һ����Ҫ���ҵ�ֵ���Ԫ�صĵ�����. 
     *          ���û���ҵ����Ԫ�أ��������ص�����last.
     */
    const_iterator find(T object) const
    {
        return std::find(_data.begin(), _data.end(), object);
    }
    
    iterator find(T object)
    {
        return std::find(_data.begin(), _data.end(), object);
    }
    
    /** ��������(vector)��λ��Ϊ'index'��Ԫ�� */
    T at(ssize_t index) const
    {
        CCASSERT( index >= 0 && index < size(), "index out of range in getObjectAtIndex()");
        return _data[index];
    }
	
    /** ��������(vector)�еĵ�һ��Ԫ��. */
    T front() const
    {
        return _data.front();
    }
    
    /** ��������(vector)�е����һ��Ԫ��. */
    T back() const
    {
        return _data.back();
    }

    /** ��������(vector)�е�һ�����Ԫ��. */
    T getRandomObject() const
    {
        if (!_data.empty())
        {
            ssize_t randIdx = rand() % _data.size();
            return *(_data.begin() + randIdx);
        }
        return nullptr;
    }

    /** ����һ��Booleanֵ��ָʾ����object�Ƿ�������(vector)��. */
    bool contains(T object) const
    {
        return( std::find(_data.begin(), _data.end(), object) != _data.end() );
    }

    /** �����������(vector)��ȣ��򷵻�true */
    bool equals(const Vector<T> &other)
    {
        ssize_t s = this->size();
        if (s != other.size())
            return false;
        
        for (ssize_t i = 0; i < s; i++)
        {
            if (this->at(i) != other.at(i))
            {
                return false;
            }
        }
        return true;
    }

    // Adds objects
    
	/** @brief ������(vector)��ĩβ���һ���µ�Ԫ�أ�λ�ڵ�ǰ���һ��Ԫ��֮��
     *  @note ��������Ὣ������size����1,
     *        ���ҽ����µ�����(vector)��size������ǰ������ʱ
     *        �˺����������Զ�Ϊ�ѷ���Ĵ洢�ռ����·���.
     */
    void pushBack(T object)
    {
        CCASSERT(object != nullptr, "The object should not be nullptr");
        _data.push_back( object );
        object->retain();
    }
    
    /** ��һ���Ѵ��ڵ�����(vector)������Ԫ�ؼӵ���ǰ����(vector)��ĩβ. */
    void pushBack(const Vector<T>& other)
    {
        for(const auto &obj : other) {
            _data.push_back(obj);
            obj->retain();
        }
    }
	
	/** @brief ���ض�����λ�ò���һ���ض�Ԫ�� 
     *  @note ͨ����ָ������'index'ǰ����һ����Ԫ��,����(vector)�ᱻ��չ
     *        ���ʹ��������size���,���ӵ���Ŀ���ڲ���Ԫ�ص���Ŀ.
     *        ���ҽ����µ�����(vector)��size������ǰ������ʱ 
     *        �˺����������Զ�Ϊ�ѷ���Ĵ洢�ռ����·���..
     */
    void insert(ssize_t index, T object)
    {
        CCASSERT(index >= 0 && index <= size(), "Invalid index!");
        CCASSERT(object != nullptr, "The object should not be nullptr");
        _data.insert((std::begin(_data) + index), object);
        object->retain();
    }
    
    // Removes Objects

	 /** ɾ������(vector)���һ��Ԫ��, 
     *  ��Ч�ؽ�������size��С1. ͬʱ��С��ɾ����������ü���
     */
    void popBack()
    {
        CCASSERT(!_data.empty(), "no objects added");
        auto last = _data.back();
        _data.pop_back();
        last->release();
    }
    
	/** @brief ɾ������(vector)�е��ض����� .
     *  @param object Ҫ��ɾ���Ķ���.
     *  @param removeAll �Ƿ�Ҫɾ����ָ�������ֵ��ͬ������Ԫ��
     *                   ���ֵΪ'false', ������ɾ�����ҵ��ĵ�һ��Ԫ��.
     */
    void eraseObject(T object, bool removeAll = false)
    {
        CCASSERT(object != nullptr, "The object should not be nullptr");
        
        if (removeAll)
        {
            for (auto iter = _data.begin(); iter != _data.end();)
            {
                if ((*iter) == object)
                {
                    iter = _data.erase(iter);
                    object->release();
                }
                else
                {
                    ++iter;
                }
            }
        }
        else
        {
            auto iter = std::find(_data.begin(), _data.end(), object);
            if (iter != _data.end())
            {
                _data.erase(iter);
                object->release();
            }
        }
    }
	
	/** @brief ͨ��������������(vector)��ɾ������. 
     *  @param position ָ������(vector)��Ҫ��ɾ���ĵ���Ԫ�صĵ�����.
     *  @return һ��ָ����λ�õ�Ԫ�صĵ����������Ԫ�ؽ������ɴ˺�������ɾ���Ķ���֮��
     *          ����������ɾ�������������һ��Ԫ�أ���ô���ص���ָ���������һ��Ԫ��֮��ĵ�����.
     */
    iterator erase(iterator position)
    {
        CCASSERT(position >= _data.begin() && position < _data.end(), "Invalid position!");
        (*position)->release();
        return _data.erase(position);
    }
    
	/** @brief ɾ������(vector)����һ�������ڵ�Ԫ�� (  [first, last)  ).
     *  @param first �������ʼ
     *  @param last ����Ľ���, 'last' ���ᱻʹ��, ֻ������ָʾ�����ĩβ.
     *  @return һ��ָ����λ�õ�Ԫ�صĵ����������Ԫ�ؽ������ɴ˺�������ɾ���Ķ���֮��.
     *          ����������ɾ�������������һ��Ԫ�أ���ô���ص���ָ���������һ��Ԫ��֮��ĵ�����.
     */
    iterator erase(iterator first, iterator last)
    {
        for (auto iter = first; iter != last; ++iter)
        {
            (*iter)->release();
        }
        
        return _data.erase(first, last);
    }
    
    /** @brief ɾ������(vector)�и���������Ԫ��.
     *  @param index ����(vector)��Ҫ��ɾ����Ԫ�ص�����.
     *  @return  һ��ָ����λ�õ�Ԫ�صĵ����������Ԫ�ؽ������ɴ˺�������ɾ���Ķ���֮��.
     *          ����������ɾ�������������һ��Ԫ�أ���ô���ص���ָ���������һ��Ԫ��֮��ĵ�����.
     */
    iterator erase(ssize_t index)
    {
        CCASSERT(!_data.empty() && index >=0 && index < size(), "Invalid index!");
        auto it = std::next( begin(), index );
        (*it)->release();
        return _data.erase(it);
    }

	/** @brief ɾ������(vector)�е�����Ԫ�� (��ЩԪ�ر�����), ֻ����һ��sizeΪ0������.
     *  @note ������(vector)�е�����Ԫ�ض��ᱻ�ͷ� (��С���ü���).
     */
    void clear()
    {
        for( auto it = std::begin(_data); it != std::end(_data); ++it ) {
            (*it)->release();
        }
        _data.clear();
    }

    // Rearranging Content

    /** ��������Ԫ�� */
    void swap(T object1, T object2)
    {
        ssize_t idx1 = getIndex(object1);
        ssize_t idx2 = getIndex(object2);

        CCASSERT(idx1>=0 && idx2>=0, "invalid object index");

        std::swap( _data[idx1], _data[idx2] );
    }

    /** ��������ָ����������Ԫ�� */
    void swap(ssize_t index1, ssize_t index2)
    {
        CCASSERT(index1 >=0 && index1 < size() && index2 >= 0 && index2 < size(), "Invalid indices");

        std::swap( _data[index1], _data[index2] );
    }

    /** ��ָ���������Ķ����滻Ϊ��һ������. */
    void replace(ssize_t index, T object)
    {
        CCASSERT(index >= 0 && index < size(), "Invalid index!");
        CCASSERT(object != nullptr, "The object should not be nullptr");
        
        _data[index]->release();
        _data[index] = object;
        object->retain();
    }

    /** ������(vector)���� */
    void reverse()
    {
        std::reverse( std::begin(_data), std::end(_data) );
    }
    
    /** ��������(vector)ʹ�÷�����ڴ��С��Ԫ�ص���Ŀ����һ�� */
    void shrinkToFit()
    {
        _data.shrink_to_fit();
    }
    
protected:
    
    /** ������(vector)�е����ж����������� */
    void addRefForAllObjects()
    {
        for(const auto &obj : _data) {
            obj->retain();
        }
    }
    
    std::vector<T> _data;
};

// end of data_structure group
/// @}

NS_CC_END

#endif // __CCVECTOR_H__
