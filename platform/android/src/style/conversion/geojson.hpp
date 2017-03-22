#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <sstream>
#include <string>

namespace mbgl {
namespace style {
namespace conversion {

static Result<GeoJSON> convertGeoJSON(const std::string jsonString) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    d.Parse(jsonString.c_str());

    if (d.HasParseError()) {
        std::stringstream message;
        message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
        return Error { message.str() };
    }

    conversion::Result<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d);
    if (!geoJSON) {
        return Error { geoJSON.error().message };
    }

    return geoJSON;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
