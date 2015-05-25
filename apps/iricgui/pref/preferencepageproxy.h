#ifndef PREFERENCEPAGEPROXY_H
#define PREFERENCEPAGEPROXY_H

#include <QWidget>
#include "preferencepage.h"
#include "../misc/networksetting.h"

namespace Ui
{
	class PreferencePageProxy;
}

class PreferencePageProxy : public PreferencePage
{
	Q_OBJECT
public:
	explicit PreferencePageProxy(QWidget* parent = nullptr);
	~PreferencePageProxy();
	void update() override;

private:
	NetworkSetting m_setting;
	Ui::PreferencePageProxy* ui;
};

#endif // PREFERENCEPAGEPROXY_H
