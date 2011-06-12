#pragma once

#include "Document.h"

template <class T>
class View
{
public:
	View(Document& doc)
		: m_document(doc)
	{
		m_connection = doc.connect(std::bind(&View<T>::UpdateHandler, this));
	}

	virtual ~View(){ m_document.disconnect(m_connection); }

private:
	void UpdateHandler()
	{
		static_cast<T*>(this)->OnDocumentUpdate();
	}

protected:
	Document&				m_document;


private:
	Document::connection_t	m_connection;
};
