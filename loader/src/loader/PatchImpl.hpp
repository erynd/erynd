﻿#pragma once

#include <Geode/loader/Hook.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include "ModImpl.hpp"
#include "ModPatch.hpp"

using namespace geode::prelude;

class Patch::Impl final : ModPatch {
public:
    Impl(void* address, ByteVector original, ByteVector patch);
    ~Impl();

    static Patch* create(void* address, const ByteVector& patch);

    Patch* m_self = nullptr;
    void* m_address;
    ByteVector m_original;
    ByteVector m_patch;

    Result<> enable();
    Result<> disable();

    uintptr_t getAddress() const;
    matjson::Value getRuntimeInfo() const;

    friend class Patch;
    friend class Mod;
};
