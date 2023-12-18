#pragma once

#include <QWidget>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>

class PlaylistModel;
class SortFilterProxyModel;

class PlaylistView : public QTableView
{
	Q_OBJECT
	public:
		PlaylistView(PlaylistModel *model, QWidget *parent);
	public slots:
		void setupColumns();
		void find(const QString &text);
		void onSelect(const QModelIndex &index);
		void onDelete();

	protected:
		void keyPressEvent(QKeyEvent *event) override;

	private:
		PlaylistModel *m_model;
		SortFilterProxyModel *m_proxyModel;
		QTimer *blinkAnimationTimer;
};
