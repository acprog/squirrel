#pragma once
#ifndef SSQ_CLASS_HEADER_H
#define SSQ_CLASS_HEADER_H

#include <functional>
#include "function.hpp"
#include "binding.hpp"

namespace ssq {
    /**
    * @brief Squirrel class object
    * @ingroup simplesquirrel
    */
    class SQUIRREL_API SSQ_API Class : public Object {
    public:
        /**
        * @brief Constructor helper class
        */
        template<class Signature>
        struct Ctor;

        template<class T, class... Args>
        struct Ctor<T(Args...)> {
            static T* allocate(Args&&... args) {
                return new T(std::forward<Args>(args)...);
            }
        };
		/**
        * @brief Creates an empty invalid class
        */
		Class();
        /**
        * @brief Destructor
        */
        virtual ~Class() = default;
        /**
        * @brief Creates a new empty class
        */
        Class(HSQUIRRELVM vm);
        /**
        * @brief Converts Object to class object
        * @throws TypeException if the Object is not type of a class
        */
        explicit Class(const Object& object);
        /**
        * @brief Copy constructor
        */
        Class(const Class& other);
        /**
        * @brief Move constructor
        */
        Class(Class&& other) NOEXCEPT;
        /**
        * @brief Swaps two classes
        */
        void swap(Class& other) NOEXCEPT;
        /**
        * @brief Finds a function in this class
        * @throws RuntimeException if VM is invalid
        * @throws NotFoundException if function was not found
        * @throws TypeException if the object found is not a function
        */
        Function findFunc(const char* name) const;
        /**
        * @brief Adds a new function type to this class
        * @param name Name of the function to add
        * @param func std::function that contains "this" pointer to the class type followed
        * by any number of arguments with any type
        * @throws RuntimeException if VM is invalid
        * @returns Function object references the added function
        */
        template <typename Return, typename Object, typename... Args>
        Function addFunc(const char* name, const std::function<Return(Object*, Args...)>& func, bool isStatic = false) {
            if (vm == nullptr) throw RuntimeException("VM is not initialised");
            Function ret(vm);
            sq_pushobject(vm, obj);
            detail::addMemberFunc(vm, name, func, isStatic);
            sq_pop(vm, 1);
            return ret;
        }
        /**
        * @brief Adds a new function type to this class
        * @param name Name of the function to add
        * @param memfunc Pointer to member function
        * @throws RuntimeException if VM is invalid
        * @returns Function object references the added function
        */
        template <typename Return, typename Object, typename... Args>
        Function addFunc(const char* name, Return(Object::*memfunc)(Args...), bool isStatic = false) {
            auto func = std::function<Return(Object*, Args...)>(std::mem_fn(memfunc));
            return addFunc(name, func, isStatic);
        }
        /**
        * @brief Adds a new function type to this class
        * @param name Name of the function to add
        * @param memfunc Pointer to constant member function
        * @throws RuntimeException if VM is invalid
        * @returns Function object references the added function
        */
        template <typename Return, typename Object, typename... Args>
        Function addFunc(const char* name, Return(Object::*memfunc)(Args...) const, bool isStatic = false) {
            auto func = std::function<Return(Object*, Args...)>(std::mem_fn(memfunc));
            return addFunc(name, func, isStatic);
        }
        /**
        * @brief Adds a new function type to this class
        * @param name Name of the function to add
        * @param lambda Lambda function that contains "this" pointer to the class type followed
        * by any number of arguments with any type
        * @throws RuntimeException if VM is invalid
        * @returns Function object references the added function
        */
#ifdef SQUNICODE
        template<typename F>
        Function addFunc(const TCHAR* name, const F& lambda, bool isStatic = false) {
            return addFunc(name, detail::make_function(lambda), isStatic);
        }
        template<typename T, typename V>
        void addVar(const FString& name, V T::* ptr, bool isStatic = false) {
            findTable("_get", tableGet, dlgGetStub);
            findTable("_set", tableSet, dlgSetStub);

            bindVar<T, V>(name, ptr, tableGet.getRaw(), varGetStub<T, V>, isStatic);
            bindVar<T, V>(name, ptr, tableSet.getRaw(), varSetStub<T, V>, isStatic);
        }
        template<typename T, typename V>
        void addConstVar(const FString& name, V T::* ptr, bool isStatic = false) {
            findTable("_get", tableGet, dlgGetStub);

            bindVar<T, V>(name, ptr, tableGet.getRaw(), varGetStub<T, V>, isStatic);
        }
#else
        template<typename F>
        Function addFunc(const char* name, const F& lambda, bool isStatic = false) {
            return addFunc(name, detail::make_function(lambda), isStatic);
        }
        template<typename T, typename V>
        void addVar(const std::string& name, V T::* ptr, bool isStatic = false) {
            findTable("_get", tableGet, dlgGetStub);
            findTable("_set", tableSet, dlgSetStub);

            bindVar<T, V>(name, ptr, tableGet.getRaw(), varGetStub<T, V>, isStatic);
            bindVar<T, V>(name, ptr, tableSet.getRaw(), varSetStub<T, V>, isStatic);
        }
        template<typename T, typename V>
        void addConstVar(const std::string& name, V T::* ptr, bool isStatic = false) {
            findTable("_get", tableGet, dlgGetStub);

            bindVar<T, V>(name, ptr, tableGet.getRaw(), varGetStub<T, V>, isStatic);
        }
#endif
        /**
        * @brief Copy assingment operator
        */
        Class& operator = (const Class& other);
        /**
        * @brief Move assingment operator
        */
        Class& operator = (Class&& other) NOEXCEPT;

        bool operator==(const Class &other) const {
          return obj._type==other.obj._type && obj._unVal.pClass == other.obj._unVal.pClass;
        }

        // get base class from current
        Class getBase() {
          sq_pushobject(vm, obj);
          sq_getbase(vm, -1);
          Object base(vm);
          if (SQ_FAILED(sq_getstackobj(vm, -1, &base.getRaw())))
            return Class();// throw TypeException("Could not get base class from squirrel stack");
          sq_addref(vm, &base.getRaw());
          sq_pop(vm, 2);
          return base.getType()==Type::CLASS ? base.toClass() : Class();
        };

        bool isDerivedFrom(const Class &parent) {
          auto base=getBase();
          while (!(base==Class())) {
            if (base==parent)
              return true;
            base=base.getBase();
          }
          return false;
        }

        void beginIteration() {
          sq_pushobject(vm, obj);
          sq_pushnull(vm);
        }

        bool nextIteration(Object& key, Object& val) {
          if (SQ_SUCCEEDED(sq_next(vm, -2))) {
            Object k(vm);
            Object v(vm);
            if (SQ_FAILED(sq_getstackobj(vm, -2, &k.getRaw()))) throw TypeException("Could not get key from squirrel stack");
            sq_addref(vm, &k.getRaw());
            if (SQ_FAILED(sq_getstackobj(vm, -1, &v.getRaw()))) throw TypeException("Could not get value from squirrel stack");
            sq_addref(vm, &v.getRaw());
            sq_pop(vm, 2);
            key = k;
            val = v;
            return true;
          }
          return false;
        }

        void EndIteration()
        {
          sq_pop(vm, 2);
        }


    protected:
#ifdef SQUNICODE
      void findTable(const FString &name, Object& table, SQFUNCTION dlg) const;
#else
        void findTable(const char* name, Object& table, SQFUNCTION dlg) const;
#endif
        static SQInteger dlgGetStub(HSQUIRRELVM vm);
        static SQInteger dlgSetStub(HSQUIRRELVM vm);

#ifdef SQUNICODE
        template<typename T, typename V>
        void bindVar(const FString& name, V T::* ptr, HSQOBJECT& table, SQFUNCTION stub, bool isStatic) {
            auto rst = sq_gettop(vm);

            sq_pushobject(vm, table);
            sq_pushstring(vm, *name, name.Len());

            auto vp = sq_newuserdata(vm, sizeof(ptr));
            std::memcpy(vp, &ptr, sizeof(ptr));

            sq_newclosure(vm, stub, 1);

            if (SQ_FAILED(sq_newslot(vm, -3, isStatic))) {
                throw TypeException("Failed to bind member variable to class");
            }

            sq_pop(vm, 1);
            sq_settop(vm, rst);
        }
#else
        template<typename T, typename V>
        void bindVar(const std::string& name, V T::* ptr, HSQOBJECT& table, SQFUNCTION stub, bool isStatic) {
            auto rst = sq_gettop(vm);

            sq_pushobject(vm, table);
            sq_pushstring(vm, name.c_str(), name.size());

            auto vp = sq_newuserdata(vm, sizeof(ptr));
            std::memcpy(vp, &ptr, sizeof(ptr));

            sq_newclosure(vm, stub, 1);

            if (SQ_FAILED(sq_newslot(vm, -3, isStatic))) {
                throw TypeException("Failed to bind member variable to class");
            }

            sq_pop(vm, 1);
            sq_settop(vm, rst);
        }
#endif

        template<typename T, typename V>
        static SQInteger varGetStub(HSQUIRRELVM vm) {
            T* ptr;
            sq_getinstanceup(vm, 1, reinterpret_cast<SQUserPointer*>(&ptr), nullptr);

            typedef V T::*M;
            M* memberPtr = nullptr;
            sq_getuserdata(vm, -1, reinterpret_cast<SQUserPointer*>(&memberPtr), nullptr);
            M member = *memberPtr;

            detail::push(vm, ptr->*member);
            return 1;
        }

        template<typename T, typename V>
        static SQInteger varSetStub(HSQUIRRELVM vm) {
            T* ptr;
            sq_getinstanceup(vm, 1, reinterpret_cast<SQUserPointer*>(&ptr), nullptr);

            typedef V T::*M;
            M* memberPtr = nullptr;
            sq_getuserdata(vm, -1, reinterpret_cast<SQUserPointer*>(&memberPtr), nullptr);
            M member = *memberPtr;

            ptr->*member = detail::pop<V>(vm, 2);
            return 0;
        }

        Object tableSet;
        Object tableGet;
    };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    namespace detail {
        template<>
        inline Class popValue(HSQUIRRELVM vm, SQInteger index){
            checkType(vm, index, OT_CLASS);
            Class val(vm);
            if (SQ_FAILED(sq_getstackobj(vm, index, &val.getRaw()))) throw TypeException("Could not get Class from squirrel stack");
            sq_addref(vm, &val.getRaw());
            return val;
        }
    }
#endif
}

#endif
