#pragma once

#include "SectionType.h"

#include <qobject.h>
#include <qstring.h>
#include <qtmetamacros.h>

class test_Section {
public:
	test_Section(const QString& data_path, const SectionType& type)
		: data_path(data_path), type(type) {}

protected:
	const QString data_path;
	const SectionType type;
};

class test_Section1 : public QObject, public test_Section {
	Q_OBJECT

public:
	test_Section1(const QString& data_path, const SectionType& type)
		: test_Section(data_path, type) {}

private:
	void loadTestData(const QString& path, const QString& item);

private slots:
	void testToHtml_data();
	void testToHtml();
};

class test_Section2 : public QObject, public test_Section {
	Q_OBJECT

public:
	test_Section2(const QString& data_path, const SectionType& type)
		: test_Section(data_path, type) {}

private:
	void loadTestData(const QString& path, const QString& item);

private slots:
	void testToHtml_data();
	void testToHtml();
};

#include "test_Section.moc"