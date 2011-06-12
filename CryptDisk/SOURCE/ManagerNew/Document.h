#pragma once

class Document
{
public:
	typedef boost::signal<void ()>  signal_t;
	typedef boost::signals::connection  connection_t;

public:

	virtual ~Document(){}

	connection_t connect(signal_t::slot_function_type subscriber)
	{
		return m_sig.connect(subscriber);
	}

	void disconnect(connection_t subscriber)
	{
		subscriber.disconnect();
	}

	void UpdateViews()
	{
		m_sig();
	}

private:
	signal_t    m_sig;
};
