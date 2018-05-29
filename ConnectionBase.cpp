#include "ConnectionBase.hpp"



void ConnectionBase::setErrMSG(string && msg)
{
	mtx->lock();
	if (errMSG[0] == ERR_MSG_DEF_VAL) {
		errMSG[0] = move(msg);
		mtx->unlock();
		return;
	}

	errMSG.push_back(move(msg));

	if (errMSG.size() > ERR_MSG_NMB)
		errMSG.pop_front();
	mtx->unlock();
}

ConnectionBase::ConnectionBase()
{
	errMSG.push_back(ERR_MSG_DEF_VAL);

	mtx = new mutex();
}

ConnectionBase::ConnectionBase(const ConnectionBase & other) : errMSG{other.errMSG}
{
	mtx = new mutex();
}

ConnectionBase::ConnectionBase(ConnectionBase && other)
{
	errMSG = move(other.errMSG);

	mtx = new mutex();

	other.errMSG = deque<string>();
}

ConnectionBase & ConnectionBase::operator=(ConnectionBase && other)
{
	if (this != &other) {

		swap(errMSG, other.errMSG);

	}

	return *this;
}

ConnectionBase::~ConnectionBase()
{
	delete mtx;
}

const string& ConnectionBase::getLastErr() const
{
	/*if (errMSG.size() == 0) {
		const string& rv = "there is no last error";
		return rv;
	}*/
	return errMSG.back();
}

const deque<string>& ConnectionBase::getErrs() const
{
	return errMSG;
}

//deque<string>::const_reverse_iterator &  ConnectionBase::getErrItBegin() const
//{
//	return errMSG.crbegin();
//}
//
//deque<string>::const_reverse_iterator &  ConnectionBase::getErrItEnd() const
//{
//	return errMSG.crend();
//}
