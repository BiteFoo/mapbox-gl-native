#include "geojson_source.hpp"

// Java -> C++ conversion
#include "../android_conversion.hpp"
#include "../conversion/filter.hpp"
#include "../conversion/geojson.hpp"

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include "../../conversion/collection.hpp"
#include "../../geojson/conversion/feature.hpp"
#include "../conversion/url_or_tileset.hpp"
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>

#include <string>

namespace mbgl {
namespace android {

    GeoJSONSource::GeoJSONSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> options)
        : Source(env, std::make_unique<mbgl::style::GeoJSONSource>(
                jni::Make<std::string>(env, sourceId),
                options ? *style::conversion::convert<style::GeoJSONOptions>(Value(env, options)) : style::GeoJSONOptions()
                )
            ) {
    }

    GeoJSONSource::GeoJSONSource(mbgl::Map& map, mbgl::style::GeoJSONSource& coreSource)
        : Source(map, coreSource) {
    }

    GeoJSONSource::~GeoJSONSource() = default;

    void GeoJSONSource::setGeoJSONString(jni::JNIEnv& env, jni::String json) {
        using namespace mbgl::style::conversion;

        // Convert the jni object
        auto converted = convertGeoJSON(jni::Make<std::string>(env, json));
        if(!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting GeoJSON string: " + converted.error().message);
            return;
        }

        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(*converted);
    }

    void GeoJSONSource::setFeatureCollection(jni::JNIEnv& env, jni::Object<geojson::FeatureCollection> jFeatures) {
        using namespace mbgl::android::geojson;

        // Convert the jni object
        auto features = FeatureCollection::convert(env, jFeatures);

        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(GeoJSON(features));
    }

    void GeoJSONSource::setFeature(jni::JNIEnv& env, jni::Object<geojson::Feature> jFeature) {
        using namespace mbgl::android::geojson;

        // Convert the jni object
        auto feature = Feature::convert(env, jFeature);

        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(GeoJSON(feature));
    }

    void GeoJSONSource::setGeometry(jni::JNIEnv& env, jni::Object<geojson::Geometry> jGeometry) {
        using namespace mbgl::android::geojson;

        // Convert the jni object
        auto geometry = Geometry::convert(env, jGeometry);

        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(GeoJSON(geometry));
    }

    void GeoJSONSource::setURL(jni::JNIEnv& env, jni::String url) {
        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setURL(jni::Make<std::string>(env, url));
    }

    jni::Array<jni::Object<geojson::Feature>> GeoJSONSource::querySourceFeatures(jni::JNIEnv& env,
                                                                        jni::Array<jni::Object<>> jfilter) {
        using namespace mbgl::android::conversion;
        using namespace mbgl::android::geojson;

        auto filter = toFilter(env, jfilter);
        auto features = source.querySourceFeatures({ {},  filter });
        return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(env, features);
    }

    jni::Class<GeoJSONSource> GeoJSONSource::javaClass;

    jni::jobject* GeoJSONSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = GeoJSONSource::javaClass.template GetConstructor<jni::jlong>(env);
        return GeoJSONSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void GeoJSONSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        GeoJSONSource::javaClass = *jni::Class<GeoJSONSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<GeoJSONSource>(
            env, GeoJSONSource::javaClass, "nativePtr",
            std::make_unique<GeoJSONSource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize",
            METHOD(&GeoJSONSource::setGeoJSONString, "nativeSetGeoJsonString"),
            METHOD(&GeoJSONSource::setFeatureCollection, "nativeSetFeatureCollection"),
            METHOD(&GeoJSONSource::setFeature, "nativeSetFeature"),
            METHOD(&GeoJSONSource::setGeometry, "nativeSetGeometry"),
            METHOD(&GeoJSONSource::setURL, "nativeSetUrl"),
            METHOD(&GeoJSONSource::querySourceFeatures, "querySourceFeatures")
        );
    }

} // namespace android
} // namespace mbgl
