#include "value.hpp"

#include "../java_types.hpp"

namespace mbgl {
namespace android {

    class ObjectDeleter {
    public:
        ObjectDeleter() = default;
        ObjectDeleter(JNIEnv& e) : env(e) {}

        void operator()(jni::jobject* p) const {
          if (p) {
              jni::DeleteLocalRef(env, p);
          }
        }

    private:
        JNIEnv& env;
    };

    // Instance

    Value::Value(jni::JNIEnv& _env, jni::jobject* _value) : env(_env), value(_value, ObjectDeleter(env)) {}

    Value::~Value() = default;

    bool Value::isNull() const {
        return value == nullptr;
    }

    bool Value::isArray() const {
        return jni::IsInstanceOf(env, value.get(), *java::ObjectArray::jclass);
    }

    bool Value::isObject() const {
        return jni::IsInstanceOf(env, value.get(), *java::Map::jclass);;
    }

    bool Value::isString() const {
        return jni::IsInstanceOf(env, value.get(), *java::String::jclass);
    }

    bool Value::isBool() const {
        return jni::IsInstanceOf(env, value.get(), *java::Boolean::jclass);
    }

    bool Value::isNumber() const {
        return jni::IsInstanceOf(env, value.get(), *java::Number::jclass);
    }

    std::string Value::toString() const {
        jni::jstring* string = reinterpret_cast<jni::jstring*>(value.get());
        return jni::Make<std::string>(env, jni::String(string));
    }

    float Value::toNumber() const {
        return jni::CallMethod<jni::jfloat>(env, value.get(), *java::Number::floatValueMethodId);
    }

    bool Value::toBool() const {
        return jni::CallMethod<jni::jboolean>(env, value.get(), *java::Boolean::booleanValueMethodId);
    }

    Value Value::get(const char* key) const {
        jni::jobject* member = jni::CallMethod<jni::jobject*>(env, value.get(), *java::Map::getMethodId, jni::Make<jni::String>(env, std::string(key)).Get());
        return Value(env, member);
    }

    int Value::getLength() const {
        auto array = (jni::jarray<jni::jobject>*) value.get();
        return jni::GetArrayLength(env, *array);
    }

    Value Value::get(const int index) const {
        auto array = (jni::jarray<jni::jobject>*) value.get();
        return Value(env, jni::GetObjectArrayElement(env, *array, index));
    }
}
}

