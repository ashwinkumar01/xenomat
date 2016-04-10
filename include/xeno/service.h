/*
 * service.h
 *
 *  Created on: 20.10.2013
 *      Author: Peter Hübel
 */

#ifndef XENO_SERVICE_H_
#define XENO_SERVICE_H_

#include <cassert>

#include "document.h"

#ifndef XENO_NO_ASIO
#include <boost/asio.hpp>
#endif

#include <boost/ref.hpp>

namespace xeno XENO_EXPORT {

extern "C"
XENO_EXPORT void xeno_declare(const char* const ns, const char* const name, context::creator builder);

extern "C"
XENO_EXPORT void xeno_nullify(const char* const ns, const char* const name, context::creator builder);

extern "C"
XENO_EXPORT action* xeno_lookup(const context&, const char* const path);

extern "C"
XENO_EXPORT action* xeno_create(context&, const char* const ns, const char* const name);

extern "C"
XENO_EXPORT action* xeno_inject(context&, action* svc);

struct XENO_EXPORT action : context {
public:
	inline virtual ~action() {};
	inline virtual void invoke(client& visitor, sequens& route) {};
	inline virtual const context& origin() const { return *this; }
//	// Allocation
//	inline void* operator new(std::size_t sz) throw (std::bad_alloc)
//	{
//		return ::operator new(sz);
//	}
//	inline void* operator new(std::size_t sz, const std::nothrow_t& nt) throw()
//	{
//		return ::operator new(sz,nt);
//	}
protected:
#ifndef XENO_NO_ASIO
	boost::asio::io_service& get_io_service() const;
#endif
};

template <class Service>
struct XENO_EXPORT ref_attribute;

template <class SERVICE = xeno::action, class CONTEXT = xeno::context>
class XENO_EXPORT service : public virtual action {
public:

	typedef SERVICE Service;
	typedef CONTEXT Context;
	typedef service<SERVICE,CONTEXT> Origin;

	inline service(Context& ctx)
	:	origin_(ctx)
	{
	}

	// The origin of this service
	inline const Context& origin() const { return origin_; }

	// How to make a service
	static inline action* create(Context& origin)
	{
		return new Service(origin);
	}

	template <class SVC = Origin::Service, class CTX = Origin::Context>
	static inline action* create(CTX& origin)
	{
		return create(static_cast<Context&>(origin));
	}

	template <class Origin>
	static inline SERVICE* lookup(Origin& origin, const char* const path)
	{
		return dynamic_cast<SERVICE*>(xeno_lookup(origin, path));
	}

	typedef boost::reference_wrapper<SERVICE> service_ref;

	template <class SVC = Service, class CTX = Context>
	static inline service_ref ref(CTX& origin, const char* const path)
	{
		return service_ref(*lookup(origin, path));
	}

	template <class AttributeService>
	inline AttributeService* ref_attr() const
	{
		return ref_attribute<AttributeService>(origin());
	}

	template <class AttributeService>
	inline typename AttributeService::service_ref ref() const
	{
		AttributeService* attr = ref_attr<AttributeService>();
		return typename AttributeService::service_ref(*attr);
	}

	template <class AttributeService>
	inline service_ref static_ref() const
	{
		return ref<AttributeService>(origin(), AttributeService::ATTRIBUTE_NAME + 1);
	}

private:
	// The context of this service
	Context& origin_;
};

template <class Service>
struct XENO_EXPORT ref_attribute
{
	template <class Context>
	inline ref_attribute(const Context& origin)
	:	service(Service::lookup(origin,
			attribute(Service::ATTRIBUTE_NAME, origin, Service::ATTRIBUTE_NAME + 1).c_str()))
	{
		assert('@'==*Service::ATTRIBUTE_NAME);
	}
	inline operator Service*() const { return service; }
private:
	Service* service;
};

template <class Service>
class declaration {
public:
	inline declaration(const char* const ns, const char* const name)
	{
		xeno_declare(ns, name, Service::create);
	}
};

template <class Service>
inline void declare(const char* const ns, const char* const name)
{
	static declaration<Service> decl(ns, name);
}

// NOTE: This for automatic registering a class.
// Just place it at the top of your service class' main,
// just before the constructor, inside the namespace
#define XENO_REGISTER(ns, name, cpp) \
void __attribute__ ((constructor)) declare_##ns##_##name() {\
	printf("declare> "#ns":"#name" as ["#cpp"] using xeno/" XENO_VERSION "\n");\
	xeno::xeno_declare(#ns, #name, cpp::create);\
}\
void __attribute__ ((destructor)) nullify_##ns##_##name() {\
	printf("nullify> "#ns":"#name" as ["#cpp"] using xeno/" XENO_VERSION "\n");\
	xeno::xeno_nullify(#ns, #name, 0);\
}\

} /* namespace xeno */

#endif//XENO_SERVICE_H_
