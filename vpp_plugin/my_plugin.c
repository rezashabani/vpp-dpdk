#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/plugin/plugin.h>
#include <vppinfra/error.h>
#include "my_plugin.h"

typedef struct {
  u8 packet_data[64];
} my_plugin_trace_t;

//
static clib_error_t *
myplugin_interface_init(vnet_main_t *vnm, u32 sw_if_index, u32 flags)
{
  u8 is_add = (flags & VNET_SW_INTERFACE_FLAG_ADMIN_UP) ? 1 : 0;

  if (is_add)
  {
    vnet_feature_enable_disable("device-input", "my_plugin", sw_if_index, is_add, 0, 0);

  }
  return (NULL);
}
VNET_SW_INTERFACE_ADMIN_UP_DOWN_FUNCTION(myplugin_interface_init);
//


static u8 * format_my_plugin_trace (u8 * s, va_list * args) {
  CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
  my_plugin_trace_t * t = va_arg (*args, my_plugin_trace_t *);

  s = format (s, "MY_PLUGIN: %U", format_hex_bytes, t->packet_data, sizeof (t->packet_data));
  return s;
}

vlib_node_registration_t my_plugin_node;

static uword my_plugin_node_fn (vlib_main_t * vm,
                                vlib_node_runtime_t * node,
                                vlib_frame_t * frame) {
  u32 n_left_from, * from, * to_next;
  my_plugin_next_t next_index;

  from = vlib_frame_vector_args (frame);
  n_left_from = frame->n_vectors;
  next_index = node->cached_next_index;

  while (n_left_from > 0) {
    u32 n_left_to_next;

    vlib_get_next_frame (vm, node, next_index, to_next, n_left_to_next);

    while (n_left_from > 0 && n_left_to_next > 0) {
      u32 bi0;
      vlib_buffer_t * b0;
      u32 next0 = MY_PLUGIN_NEXT_INTERFACE_OUTPUT;

      bi0 = from[0];
      to_next[0] = bi0;
      from += 1;
      to_next += 1;
      n_left_from -= 1;
      n_left_to_next -= 1;

      b0 = vlib_get_buffer (vm, bi0);

      u8 * payload = vlib_buffer_get_current (b0) + 14 + 20 + 8;  // Adjust as needed
      u8 * buffer_end = vlib_buffer_get_tail (b0);

      // Ensure the payload is large enough
      if (payload + 8 <= buffer_end) {
        for (int i = 0; i < 8; i++) {
          payload[i] = 0x00;  // Set payload to 0x00
        }
      } else {
        clib_warning ("Payload extends beyond buffer end");
      }

      if (PREDICT_FALSE((node->flags & VLIB_NODE_FLAG_TRACE) && (b0->flags & VLIB_BUFFER_IS_TRACED))) {
        my_plugin_trace_t * t = vlib_add_trace (vm, node, b0, sizeof (*t));
        clib_memcpy (t->packet_data, vlib_buffer_get_current (b0), sizeof (t->packet_data));
      }

      vlib_validate_buffer_enqueue_x1 (vm, node, next_index, to_next, n_left_to_next, bi0, next0);
    }

    vlib_put_next_frame (vm, node, next_index, n_left_to_next);
  }

  return frame->n_vectors;
}

VLIB_REGISTER_NODE (my_plugin_node) = {
  .function = my_plugin_node_fn,
  .name = "my_plugin",
  .vector_size = sizeof (u32),
  .format_trace = format_my_plugin_trace,
  .type = VLIB_NODE_TYPE_INTERNAL,
  .n_errors = 0,
  .error_strings = 0,
  .n_next_nodes = 1,
  .next_nodes = {
    [MY_PLUGIN_NEXT_INTERFACE_OUTPUT] = "interface-output",
  },
};

VLIB_PLUGIN_REGISTER () = {
  .version = "1.0",
  .description = "My custom VPP plugin",
};
