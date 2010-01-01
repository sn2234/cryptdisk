///////////////////////////////////////////////////////////
//  IDiskParameters.h
//  Implementation of the Interface IDiskParameters
//  Created on:      11-dec-2009 1:29:46
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_C1201609_DCE8_45b9_968E_2E1987F4DECF__INCLUDED_)
#define EA_C1201609_DCE8_45b9_968E_2E1987F4DECF__INCLUDED_

/**
 * Exposes common parameters for both v3 and v4 disks.
 */
class IDiskParameters
{
public:
	virtual ~IDiskParameters(){}
	virtual const wchar_t* getTextVersion() const =0;
	virtual unsigned int getVersion() const =0;
};
#endif // !defined(EA_C1201609_DCE8_45b9_968E_2E1987F4DECF__INCLUDED_)
