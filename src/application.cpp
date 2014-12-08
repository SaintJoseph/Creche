#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include <QApplication>
#include "application.h"
#include "func_name.h"

QTranslator* Application::current = 0;
Translators Application::translators;

Application::Application(int& argc, char* argv[])
	: QApplication(argc, argv)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    loadTranslations(":/");
    setLanguage("en_GB");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

Application::~Application()
{
}

void Application::loadTranslations(const QString& dir)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    loadTranslations(QDir(dir));
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void Application::loadTranslations(const QDir& dir)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    // <language>_<country>.qm
	QString filter = "*_*.qm";
	QDir::Filters filters = QDir::Files | QDir::Readable;
	QDir::SortFlags sort = QDir::Name;
	QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);
	foreach (QFileInfo file, entries)
	{
		// pick country and language out of the file name
		QStringList parts = file.baseName().split("_");
		QString language = parts.at(parts.count() - 2).toLower();
		QString country  = parts.at(parts.count() - 1).toUpper();

		// construct and load translator
		QTranslator* translator = new QTranslator(instance());
		if (translator->load(file.absoluteFilePath()))
		{
			QString locale = language + "_" + country;
			translators.insert(locale, translator);
		}
	}
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

const QStringList Application::availableLanguages()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    // the content won't get copied thanks to implicit sharing and constness
	return QStringList(translators.keys());
}

void Application::setLanguage(const QString& locale)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    // remove previous
	if (current)
	{
		removeTranslator(current);
	}

	// install new
	current = translators.value(locale, 0);
	if (current)
	{
		installTranslator(current);
	}
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

const QString Application::currentLanguage()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //récupère la langue courrante
    return QString(translators.key(current));
}
