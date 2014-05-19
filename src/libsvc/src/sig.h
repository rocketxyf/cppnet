#ifndef __SIG_H__
#define __SIG_H__

#include "interface.h"

class DfEventHandler: public ACE_Event_Handler
{
public:
	int handle_signal(int signum, siginfo_t*, ucontext_t*);
};

class Sig : public mmg::Sig
{
public:
	Sig();

	~Sig();

	void set_sig_handle(ACE_Event_Handler *handler);

private:
	DfEventHandler *dfe_;
};

#endif
