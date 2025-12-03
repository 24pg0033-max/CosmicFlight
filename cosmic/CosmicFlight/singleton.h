#pragma once
template<class T>
class Singleton
{
protected:
    //  コンストラクタの外部での使用を禁止
    Singleton() = default;
    virtual ~Singleton() = default;
    // コピーコンストラクタの使用を禁止
    Singleton(const Singleton&) = delete;
    Singleton& operator =(const Singleton&) = delete;

public:
    static void Build()
    {
        assert(m_pInstance == nullptr);
        m_pInstance = NEW T;
    }
    static void Release()
    {
        if (m_pInstance) {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }
    static T& Instance()
    {
        return *m_pInstance;
    }
private:
    static T* m_pInstance;
};

template<class T>
T* Singleton<T>::m_pInstance = nullptr;