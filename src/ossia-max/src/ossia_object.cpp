#include "ossia_object.hpp"
#include "ossia-max.hpp"

using namespace ossia::max;

extern "C" void ossia_ossia_setup()
{
  t_class* c = class_new(
      "ossia", (method)ossia_object::create, (method)ossia_object::destroy,
      (long)sizeof(ossia::max::ossia_object), 0L, A_GIMME, 0);

  node_base::class_setup(c);
  class_addmethod(c, (method)device::expose, "expose", A_GIMME, 0);
  class_addmethod(c, (method)device::name, "name", A_GIMME, 0);
  class_addmethod(c, (method)ossia_object::notify,"notify", A_CANT, 0);

  CLASS_ATTR_SYM(
        c, "log_level", 0, ossia_object, m_log_level);
  std::stringstream lvl_list;
  for(auto lvl : SPDLOG_LEVEL_NAMES)
      lvl_list << lvl << " ";
  CLASS_ATTR_ENUM(c, "log_level", 0, lvl_list.str().c_str());
  CLASS_ATTR_LABEL(c, "log_level", 0, "Log Level");
  CLASS_ATTR_DEFAULT(c, "log_level", 0, "error");

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia::max::ossia_max::instance();
  ossia_library.ossia_ossia_class = c;
}

namespace ossia
{
namespace max
{

void* ossia_object::create(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia::max::ossia_max::instance();

  auto x = make_ossia<ossia_object>();

  x->m_dumpout
      = outlet_new(x, NULL); // anything outlet to dump device state
  x->m_device = ossia_library.get_default_device();
  x->m_otype = object_class::device;
  x->m_name = gensym(x->m_device->get_name().c_str());
  x->m_matchers.emplace_back(std::make_shared<matcher>(&x->m_device->get_root_node(), (object_base*)nullptr));

  x->m_device->on_parameter_created.connect<&device_base::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.connect<&device_base::on_parameter_deleted_callback>(x);

  x->m_log_level = gensym("error");

  if (argc > 0 && argv[0].a_type == A_SYM){
    x->m_name = argv[0].a_w.w_sym;
    x->m_device->set_name(x->m_name->s_name);
  }

  // inhibit loadbang for ossia object
  x->m_registered = true;

  return (x);
}

void ossia_object::destroy(ossia_object *x)
{
  x->m_device->on_parameter_created.disconnect<&device_base::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.disconnect<&device_base::on_parameter_deleted_callback>(x);

  ossia_max::instance().devices.remove_all(x);

  outlet_delete(x->m_dumpout);
  x->~ossia_object();
}

t_max_err ossia_object::notify(ossia_object *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("log_level") )
    {
      ossia_max::instance().set_log_level(x->m_log_level);
    }
  }
  return 0;
}

} // namespace max
} // namespace ossia
