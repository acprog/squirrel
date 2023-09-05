#pragma once
#ifndef SSQ_EXCEPTIONS_HEADER_H
#define SSQ_EXCEPTIONS_HEADER_H

#include <string>
#include <sstream>
#include "type.hpp"
#include <CoreMinimal.h>

namespace ssq {
    /**
    * @brief Raw exception
    * @ingroup simplesquirrel
    */
#ifdef SQUNICODE
  class Exception {
  public:
    Exception(const FString& msg) :message(msg) {
    }
    virtual ~Exception() {}
    virtual const FString& what() const throw() {
      return message;
    }
  protected:
    const FString message;
  };
#else
    class Exception : public std::wexception {
      public:
        Exception(const char* msg):message(msg) {

        }
        virtual const char* what() const throw() override {
            return message;
        }
    private:
        const char* message;
    };
#endif
    /**
    * @brief Not Found exception thrown if object with a given name does not exist
    * @ingroup simplesquirrel
    */
    class NotFoundException: public Exception {
#ifdef SQUNICODE
    public:
        NotFoundException(const FString &msg):Exception("Not found: " + msg) {
        }

        virtual const FString &what() const throw() override {
            return message;
        }
    private:
//        FString message;
#else
    public:
        NotFoundException(const char* msg):Exception(msg) {
            std::stringstream ss;
            ss << "Not found: " << msg;
            message = ss.str();
        }

        virtual const char* what() const throw() override {
            return message.c_str();
        }
    private:
        std::string message;
#endif
    };
    /**
    * @brief Compile exception thrown during compilation
    * @ingroup simplesquirrel
    */
    class CompileException: public Exception {
#ifdef SQUNICODE
    public:
        CompileException(const FString &msg):Exception(msg) { 
        }

        CompileException(const FString &msg, const FString &source, int line, int column)
            :Exception("Compile error at " + source + ":" + FString::FromInt(line) + ":" + FString::FromInt(column) + " " + msg) 
        {
        }
        
        virtual const FString &what() const throw() override {
            return message;
        }
    private:
//        FString message;
#else
    public:
        CompileException(const char* msg):Exception(msg) { 
            message = std::string(msg);
        }

        CompileException(const char* msg, const char* source, int line, int column):Exception(msg) { 
            std::stringstream ss;
            ss << "Compile error at " << source << ":" << line << ":" << column << " " << msg;
            message = ss.str();
        }
        
        virtual const char* what() const throw() override {
            return message.c_str();
        }
    private:
        std::string message;
#endif
    };
    /**
    * @brief Type exception thrown if casting between squirrel and C++ objects failed
    * @ingroup simplesquirrel
    */
    class TypeException: public Exception {
#ifdef SQUNICODE
    public:
        TypeException(const FString &msg):Exception(msg) {
        }

        TypeException(const FString &msg, const FString &expected, const FString &got)
          :Exception("Type error " + msg + " expected: " + expected + " got: " + got) 
        {
        }

        virtual const FString &what() const throw() override {
            return message;
        }
    private:
//        FString message;
#else
    public:
        TypeException(const char* msg):Exception(msg) {
            message = std::string(msg);
        }

        TypeException(const char* msg, const char* expected, const char* got):Exception(msg) {
            std::stringstream ss;
            ss << "Type error " << msg << " expected: " << expected << " got: " << got;
            message = ss.str();
        }

        virtual const char* what() const throw() override {
            return message.c_str();
        }
    private:
        std::string message;
#endif
    };
    /**
    * @brief Runtime exception thrown if something went wrong during execution
    * @ingroup simplesquirrel
    */
    class RuntimeException: public Exception {
#ifdef SQUNICODE
    public:
        RuntimeException(const FString &msg):Exception(msg) {
        }

        RuntimeException(const FString &msg, const FString &source, const FString &func, int line)
          :Exception("Runtime error at (" + func + ") " + source + ":" + FString::FromInt(line) + ": " + msg) 
        {
        }

        virtual const FString &what() const throw() override {
            return message;
        }
    private:
//        FString message;
#else
    public:
        RuntimeException(const char* msg):Exception(msg) {
            message = std::string(msg);
        }

        RuntimeException(const char* msg, const char* source, const char* func, int line):Exception(msg) {
            std::stringstream ss;
            ss << "Runtime error at (" << func << ") " << source << ":" << line << ": " << msg;
            message = ss.str();
        }

        virtual const char* what() const throw() override {
            return message.c_str();
        }
    private:
        std::string message;
#endif
    };
}

#endif