#ifndef SOLVERCONSOLEWINDOW_IMPL_H
#define SOLVERCONSOLEWINDOW_IMPL_H

#include "../solverconsolewindow.h"

class SolverConsoleWindow::Impl
{
public:
	Impl(iRICMainWindowInterface* mainW, SolverConsoleWindow* w);

	QProcess* m_process;
	bool m_solverKilled;

	iRICMainWindowInterface* m_iricMainWindow;

private:
	SolverConsoleWindow* m_window;
};

#endif // SOLVERCONSOLEWINDOW_IMPL_H
