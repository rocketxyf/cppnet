#include "sig.h"
#include <ace/Reactor.h>
#include <ace/Signal.h>

int DfEventHandler::handle_signal(int signum, siginfo_t*, ucontext_t*)
{
	ACE_Reactor::instance()->end_event_loop();
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Sig::Sig()
{
	dfe_ = new DfEventHandler();
	set_sig_handle(dfe_);
}

Sig::~Sig()
{
	delete dfe_;
	dfe_ = 0;
	ACE_Sig_Set sig_set;
	sig_set.sig_add (SIGINT);
	sig_set.sig_add (SIGQUIT);
	sig_set.sig_add (SIGTERM);
	sig_set.sig_add (SIGSEGV);
	ACE_Reactor::instance()->register_handler(sig_set, 0);
}

void Sig::set_sig_handle(ACE_Event_Handler *handler)
{
	ACE_Sig_Set sig_set;
	sig_set.sig_add (SIGINT);
	sig_set.sig_add (SIGQUIT);
	sig_set.sig_add (SIGTERM);
	sig_set.sig_add (SIGSEGV);
	ACE_Reactor::instance()->register_handler(sig_set, handler);
}
