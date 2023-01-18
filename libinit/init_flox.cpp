/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
*/

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>
#include <android-base/logging.h>
#include <vector>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

std::vector<string> ro_props_default_source_order = {
    "",
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
};

void property_override(string prop, string value) {
    auto pi = (prop_info*) __system_property_find(prop.c_str());

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}

void set_ro_build_prop(const string &prop, const string &value, bool product = true) {
    string prop_name;

    for (const auto &source : ro_props_default_source_order) {
        if (product)
            prop_name = "ro.product." + source + prop;
        else
            prop_name = "ro." + source + "build." + prop;

        property_override(prop_name.c_str(), value.c_str());
    }
}

void set_build_fingerprint(string device, string name) {
    string build_fingerprint;

    build_fingerprint = "google/" + name + "/" + device + ":6.0.1/MOB30X/3036618:user/release-keys";
    set_ro_build_prop("fingerprint", build_fingerprint, false);
    property_override("ro.bootimage.build.fingerprint", build_fingerprint);

    set_ro_build_prop("name", name);
    set_ro_build_prop("name", name, false);
    set_ro_build_prop("device", device);
    set_ro_build_prop("device", device, false);
    set_ro_build_prop("model", name);
    set_ro_build_prop("product", name, false);
}

void vendor_load_properties() {
    string band;

    band = GetProperty("ro.boot.baseband", "");
     if (band == "mdm") {
        set_build_fingerprint("debx", "razorg");
    } else if (band == "apq") {
        set_build_fingerprint("flox", "razor");
    } else {
        LOG(ERROR) << __func__ << ": Coudn't indentify model!";
    }
}