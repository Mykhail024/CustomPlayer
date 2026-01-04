#pragma once

template <typename T> class ISingleton
{
    public:
        ISingleton(ISingleton &&) = delete;
        ISingleton(const ISingleton &) = delete;
        ISingleton &operator=(const ISingleton &) = delete;

        static T *instance()
        {
            static T instance;
            return &instance;
        }

    protected:
        ISingleton() = default;
        ~ISingleton() = default;
};
