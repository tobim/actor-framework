/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2020 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <utility>

#include "caf/atom.hpp"
#include "caf/detail/pp.hpp"
#include "caf/detail/squashed_int.hpp"
#include "caf/fwd.hpp"
#include "caf/meta/type_name.hpp"
#include "caf/timespan.hpp"
#include "caf/timestamp.hpp"

namespace caf {

/// Internal representation of a type ID.
using type_id_t = uint16_t;

/// Maps the type `T` to a globally unique ID.
template <class T>
struct type_id;

/// Maps the globally unique ID `V` to a type (inverse to ::type_id).
/// @relates type_id
template <type_id_t V>
struct type_by_id;

/// Convenience type that resolves to `type_name_by_id<type_id_v<T>>`.
template <class T>
struct type_name;

/// Convenience specialization that enables generic code to not treat `void`
/// manually.
template <>
struct type_name<void> {
  static constexpr const char* value = "void";
};

/// Maps the globally unique ID `V` to a type name.
template <type_id_t V>
struct type_name_by_id;

/// The first type ID not reserved by CAF and its modules.
constexpr type_id_t first_custom_type_id = 200;

} // namespace caf

/// Starts a code block for registering custom types to CAF. Stores the first ID
/// for the project as `caf::id_block::${project_name}_first_type_id`. Usually,
/// users should use `caf::first_custom_type_id` as `first_id`. However, this
/// mechanism also enables projects to append IDs to a block of another project.
/// If two projects are developed separately to avoid dependencies, they only
/// need to define sufficiently large offsets to guarantee collision-free IDs.
/// CAF supports gaps in the ID space.
///
/// @note CAF reserves all names with the suffix `_module`. For example, core
///       module uses the project name `core_module`.
#define CAF_BEGIN_TYPE_ID_BLOCK(project_name, first_id)                        \
  namespace caf {                                                              \
  namespace id_block {                                                         \
  constexpr type_id_t project_name##_type_id_counter_init = __COUNTER__;       \
  constexpr type_id_t project_name##_first_type_id = first_id;                 \
  }                                                                            \
  }

#ifdef CAF_MSVC
/// Assigns the next free type ID to `fully_qualified_name`.
#  define CAF_ADD_TYPE_ID(project_name, fully_qualified_name)                  \
    namespace caf {                                                            \
    template <>                                                                \
    struct type_id<CAF_PP_EXPAND fully_qualified_name> {                       \
      static constexpr type_id_t value                                         \
        = id_block::project_name##_first_type_id                               \
          + (CAF_PP_CAT(CAF_PP_COUNTER, ())                                    \
             - id_block::project_name##_type_id_counter_init - 1);             \
    };                                                                         \
    template <>                                                                \
    struct type_by_id<type_id<CAF_PP_EXPAND fully_qualified_name>::value> {    \
      using type = CAF_PP_EXPAND fully_qualified_name;                         \
    };                                                                         \
    template <>                                                                \
    struct type_name<CAF_PP_EXPAND fully_qualified_name> {                     \
      static constexpr const char* value                                       \
        = CAF_PP_STR(CAF_PP_EXPAND fully_qualified_name);                      \
    };                                                                         \
    template <>                                                                \
    struct type_name_by_id<type_id<CAF_PP_EXPAND fully_qualified_name>::value> \
      : type_name<CAF_PP_EXPAND fully_qualified_name> {};                      \
    }
#else
#  define CAF_ADD_TYPE_ID(project_name, fully_qualified_name)                  \
    namespace caf {                                                            \
    template <>                                                                \
    struct type_id<CAF_PP_EXPAND fully_qualified_name> {                       \
      static constexpr type_id_t value                                         \
        = id_block::project_name##_first_type_id                               \
          + (__COUNTER__ - id_block::project_name##_type_id_counter_init - 1); \
    };                                                                         \
    template <>                                                                \
    struct type_by_id<type_id<CAF_PP_EXPAND fully_qualified_name>::value> {    \
      using type = CAF_PP_EXPAND fully_qualified_name;                         \
    };                                                                         \
    template <>                                                                \
    struct type_name<CAF_PP_EXPAND fully_qualified_name> {                     \
      static constexpr const char* value                                       \
        = CAF_PP_STR(CAF_PP_EXPAND fully_qualified_name);                      \
    };                                                                         \
    template <>                                                                \
    struct type_name_by_id<type_id<CAF_PP_EXPAND fully_qualified_name>::value> \
      : type_name<CAF_PP_EXPAND fully_qualified_name> {};                      \
    }
#endif

/// Creates a new tag type (atom) and assigns the next free type ID to it.
#define CAF_ADD_ATOM(project_name, atom_namespace, atom_name, atom_text)       \
  namespace atom_namespace {                                                   \
  using atom_name = caf::atom_constant<caf::atom(atom_text)>;                  \
  constexpr atom_name atom_name##_v = atom_name{};                             \
  }

/// Finalizes a code block for registering custom types to CAF. Defines a struct
/// `caf::type_id::${project_name}` with two static members `begin` and `end`.
/// The former stores the first assigned type ID. The latter stores the last
/// assigned type ID + 1.
#define CAF_END_TYPE_ID_BLOCK(project_name)                                    \
  namespace caf {                                                              \
  namespace id_block {                                                         \
  constexpr type_id_t project_name##_last_type_id                              \
    = project_name##_first_type_id                                             \
      + (__COUNTER__ - project_name##_type_id_counter_init - 2);               \
  struct project_name {                                                        \
    static constexpr type_id_t begin = project_name##_first_type_id;           \
    static constexpr type_id_t end = project_name##_last_type_id + 1;          \
  };                                                                           \
  }                                                                            \
  }

CAF_BEGIN_TYPE_ID_BLOCK(core_module, 0)

  // -- C types

  CAF_ADD_TYPE_ID(core_module, (bool) )
  CAF_ADD_TYPE_ID(core_module, (double) )
  CAF_ADD_TYPE_ID(core_module, (float) )
  CAF_ADD_TYPE_ID(core_module, (int16_t))
  CAF_ADD_TYPE_ID(core_module, (int32_t))
  CAF_ADD_TYPE_ID(core_module, (int64_t))
  CAF_ADD_TYPE_ID(core_module, (int8_t))
  CAF_ADD_TYPE_ID(core_module, (long double) )
  CAF_ADD_TYPE_ID(core_module, (uint16_t))
  CAF_ADD_TYPE_ID(core_module, (uint32_t))
  CAF_ADD_TYPE_ID(core_module, (uint64_t))
  CAF_ADD_TYPE_ID(core_module, (uint8_t))

  // -- STL types

  CAF_ADD_TYPE_ID(core_module, (std::string))
  CAF_ADD_TYPE_ID(core_module, (std::u16string))
  CAF_ADD_TYPE_ID(core_module, (std::u32string))
  CAF_ADD_TYPE_ID(core_module, (std::set<std::string>) )

  // -- CAF types

  CAF_ADD_TYPE_ID(core_module, (caf::actor))
  CAF_ADD_TYPE_ID(core_module, (caf::actor_addr))
  CAF_ADD_TYPE_ID(core_module, (caf::atom_value))
  CAF_ADD_TYPE_ID(core_module, (caf::config_value))
  CAF_ADD_TYPE_ID(core_module, (caf::dictionary<caf::config_value>) )
  CAF_ADD_TYPE_ID(core_module, (caf::down_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::downstream_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::duration))
  CAF_ADD_TYPE_ID(core_module, (caf::error))
  CAF_ADD_TYPE_ID(core_module, (caf::exit_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::exit_reason))
  CAF_ADD_TYPE_ID(core_module, (caf::group))
  CAF_ADD_TYPE_ID(core_module, (caf::group_down_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::message))
  CAF_ADD_TYPE_ID(core_module, (caf::message_id))
  CAF_ADD_TYPE_ID(core_module, (caf::node_id))
  CAF_ADD_TYPE_ID(core_module, (caf::open_stream_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::pec))
  CAF_ADD_TYPE_ID(core_module, (caf::sec))
  CAF_ADD_TYPE_ID(core_module, (caf::strong_actor_ptr))
  CAF_ADD_TYPE_ID(core_module, (caf::timeout_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::timespan))
  CAF_ADD_TYPE_ID(core_module, (caf::timestamp))
  CAF_ADD_TYPE_ID(core_module, (caf::unit_t))
  CAF_ADD_TYPE_ID(core_module, (caf::upstream_msg))
  CAF_ADD_TYPE_ID(core_module, (caf::uri))
  CAF_ADD_TYPE_ID(core_module, (caf::weak_actor_ptr))
  CAF_ADD_TYPE_ID(core_module, (std::vector<caf::actor>) )
  CAF_ADD_TYPE_ID(core_module, (std::vector<caf::actor_addr>) )
  CAF_ADD_TYPE_ID(core_module, (std::vector<caf::config_value>) )
  CAF_ADD_TYPE_ID(core_module, (std::vector<caf::strong_actor_ptr>) )
  CAF_ADD_TYPE_ID(core_module, (std::vector<caf::weak_actor_ptr>) )
  CAF_ADD_TYPE_ID(core_module, (std::vector<std::pair<std::string, message>>) )

  // -- predefined atoms

  CAF_ADD_ATOM(core_module, caf, add_atom, "add")
  CAF_ADD_ATOM(core_module, caf, close_atom, "close")
  CAF_ADD_ATOM(core_module, caf, connect_atom, "connect")
  CAF_ADD_ATOM(core_module, caf, contact_atom, "contact")
  CAF_ADD_ATOM(core_module, caf, delete_atom, "delete")
  CAF_ADD_ATOM(core_module, caf, demonitor_atom, "demonitor")
  CAF_ADD_ATOM(core_module, caf, div_atom, "div")
  CAF_ADD_ATOM(core_module, caf, flush_atom, "flush")
  CAF_ADD_ATOM(core_module, caf, forward_atom, "forward")
  CAF_ADD_ATOM(core_module, caf, get_atom, "get")
  CAF_ADD_ATOM(core_module, caf, idle_atom, "idle")
  CAF_ADD_ATOM(core_module, caf, join_atom, "join")
  CAF_ADD_ATOM(core_module, caf, leave_atom, "leave")
  CAF_ADD_ATOM(core_module, caf, link_atom, "link")
  CAF_ADD_ATOM(core_module, caf, migrate_atom, "migrate")
  CAF_ADD_ATOM(core_module, caf, monitor_atom, "monitor")
  CAF_ADD_ATOM(core_module, caf, mul_atom, "mul")
  CAF_ADD_ATOM(core_module, caf, ok_atom, "ok")
  CAF_ADD_ATOM(core_module, caf, open_atom, "open")
  CAF_ADD_ATOM(core_module, caf, pending_atom, "pending")
  CAF_ADD_ATOM(core_module, caf, ping_atom, "ping")
  CAF_ADD_ATOM(core_module, caf, pong_atom, "pong")
  CAF_ADD_ATOM(core_module, caf, publish_atom, "publish")
  CAF_ADD_ATOM(core_module, caf, publish_udp_atom, "pub_udp")
  CAF_ADD_ATOM(core_module, caf, put_atom, "put")
  CAF_ADD_ATOM(core_module, caf, receive_atom, "receive")
  CAF_ADD_ATOM(core_module, caf, redirect_atom, "redirect")
  CAF_ADD_ATOM(core_module, caf, reset_atom, "reset")
  CAF_ADD_ATOM(core_module, caf, resolve_atom, "resolve")
  CAF_ADD_ATOM(core_module, caf, spawn_atom, "spawn")
  CAF_ADD_ATOM(core_module, caf, stream_atom, "stream")
  CAF_ADD_ATOM(core_module, caf, sub_atom, "sub")
  CAF_ADD_ATOM(core_module, caf, subscribe_atom, "subscribe")
  CAF_ADD_ATOM(core_module, caf, sys_atom, "sys")
  CAF_ADD_ATOM(core_module, caf, tick_atom, "tick")
  CAF_ADD_ATOM(core_module, caf, timeout_atom, "timeout")
  CAF_ADD_ATOM(core_module, caf, unlink_atom, "unlink")
  CAF_ADD_ATOM(core_module, caf, unpublish_atom, "unpublish")
  CAF_ADD_ATOM(core_module, caf, unpublish_udp_atom, "unpub_udp")
  CAF_ADD_ATOM(core_module, caf, unsubscribe_atom, "unsubscrib")
  CAF_ADD_ATOM(core_module, caf, update_atom, "update")
  CAF_ADD_ATOM(core_module, caf, wait_for_atom, "wait_for")

CAF_END_TYPE_ID_BLOCK(core_module)

namespace caf {
namespace detail {

static constexpr type_id_t io_module_begin = id_block::core_module::end;

static constexpr type_id_t io_module_end = io_module_begin + 19;

static constexpr type_id_t net_module_begin = io_module_end;

static constexpr type_id_t net_module_end = net_module_begin + 1;

static_assert(net_module_end <= first_custom_type_id,
              "net_module_end > first_custom_type_id");

} // namespace detail
} // namespace caf