#ifndef WBA_LOG_H
#define WBA_LOG_H
/******************************************************************************

Coral Tree wba/log.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

typedef GPL_Delivery WBA_Delivery;
typedef GPL_DeliveryPrintf WBA_DeliveryPrintf;
typedef GPL_DeliveryNoop WBA_DeliveryNoop;

#ifdef wbaLog
#undef wbaLog
#endif
#define wbaLog GPL_LOG
#ifdef wbaDLog
#undef wbaDLog
#endif
#define wbaDLog GPL_DLOG

extern GPL_Delivery *wbaWarningDelivery;
extern GPL_Delivery *wbaErrorDelivery;

extern void wbaPreEnvLoad(const char *module);
extern void wbaPostEnvLoad(const char *module);

#define WBA_AWARECLASS(module) module##_Aware
#define WBA_AWARE(module)								\
class WBA_AWARECLASS(module)							\
{														\
	public:												\
		WBA_AWARECLASS(module)()						\
		{												\
			wbaPreEnvLoad(#module);						\
		}												\
		virtual ~WBA_AWARECLASS(module)()				\
		{												\
			wbaPostEnvLoad(#module);					\
		}												\
};

WBA_AWARE(WBA);

/* ADD GPL SINCE IT CAN'T DO IT ITSELF */
WBA_AWARE(GPL);

#define WBA_MODULE_INIT_INSTANCE(module) module##_INIT
#define WBA_INIT_DEFINITION(module)  GPL_INIT_DEFINITION(module)
#define WBA_INIT_DECLARATION(module)										\
class GPL_INIT_CLASSNAME(module) : public GPL_INIT_CLASSNAME(WBA)			\
{																			\
	GPL_INIT_CONSTRUCT(module);												\
	GPL_INIT_METHOD(module);												\
};																			\
static GPL_INIT_CLASSNAME(module) WBA_MODULE_INIT_INSTANCE(module);			\
GPL_INIT_DECLARATION(module);												\
WBA_AWARE(module);

#endif
