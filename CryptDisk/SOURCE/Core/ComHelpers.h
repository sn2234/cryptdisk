
#pragma once

template<bool ThrowException>
struct HResultChecker
{
	static void Check(HRESULT hr);
};

template<> struct HResultChecker<false>
{
	static void Check(HRESULT hr)
	{
		hr;
	}
};

template<> struct HResultChecker<true>
{
static void Check(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw boost::system::system_error(boost::system::error_code(hr, boost::system::system_category()));
	}
}
};


/**
* Use this class instead HRESULT in order to the assignement operator be
* tested. In case of failure, the funcion AtlThrow() will be called.
*
* @sa AtlThrow(), CAtlException.
*/
template<bool ThrowException>
class HRESULTEX
{
public:
	/// Test the HRESULT in the constructor.
	HRESULTEX(HRESULT hr = S_OK)    { Assign(hr); }

	/// Test failure of the received hr. If FAILED(hr), the function 
	/// AtlThrow() will be called.
	HRESULTEX &operator = (HRESULT hr)
	{
		Assign(hr);
		return *this;
	}

	/**
	* Retrieves the error desription of the HRESULT member.
	* @return string message for the HRESULT.
	*
	* @author ddarko (comment from CodeProject)
	* @date 2005-09
	*/
	LPCTSTR ErrorMessage()
	{
		// a lot of code
	}

	/// Extractor of the stored HRESULT.
	operator HRESULT() { return m_hr; }

private:
	void Assign(HRESULT hr) // throw( CAtlException )
	{
		HResultChecker<ThrowException>::Check(m_hr = hr);
	}

	HRESULT m_hr; // the stored HRESULT
	std::basic_string<TCHAR> m_desc; // error description
};

/// Throw exception version.
typedef HRESULTEX<true> HResult;

// No-Throw exception version.
typedef HRESULTEX<false> HResultSafe;
