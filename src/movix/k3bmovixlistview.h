/***************************************************************************
 *   Copyright (C) 2002 by Sebastian Trueg                                 *
 *   trueg@k3b.org                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef _K3B_MOVIX_LISTVIEW_H_
#define _K3B_MOVIX_LISTVIEW_H_

#include <tools/k3blistview.h>

#include <qmap.h>


class K3bMovixDoc;
class K3bMovixFileItem;
class K3bFileItem;


class K3bMovixListViewItem : public K3bListViewItem
{
 public:
  K3bMovixListViewItem( K3bMovixDoc* doc, K3bMovixFileItem*, QListView* parent, QListViewItem* after );
  K3bMovixListViewItem( K3bMovixDoc* doc, K3bMovixFileItem*, QListViewItem* parent );
  ~K3bMovixListViewItem();

  K3bMovixFileItem* fileItem() const { return m_fileItem; }
  K3bMovixDoc* doc() const { return m_doc; }

  virtual bool isMovixFileItem() const { return true; }

 private:
  K3bMovixDoc* m_doc;
  K3bMovixFileItem* m_fileItem;
};


class K3bMovixFileViewItem : public K3bMovixListViewItem
{
 public:
  K3bMovixFileViewItem( K3bMovixDoc* doc, K3bMovixFileItem*, QListView* parent, QListViewItem* );

  QString text( int ) const;
  void setText(int col, const QString& text );

  /** always sort according to the playlist order */
  QString key( int, bool ) const;
};

class K3bMovixSubTitleViewItem : public K3bMovixListViewItem
{
 public:
  K3bMovixSubTitleViewItem( K3bMovixDoc*, K3bMovixFileItem* item, K3bMovixListViewItem* parent );
  ~K3bMovixSubTitleViewItem();

  QString text( int ) const;

  bool isMovixFileItem() const { return false; }
};


class K3bMovixListView : public K3bListView
{
  Q_OBJECT

 public:
  K3bMovixListView( K3bMovixDoc* doc, QWidget* parent = 0, const char* name = 0 );
  ~K3bMovixListView();

  QDragObject* dragObject();

 protected:
  bool acceptDrag(QDropEvent* e) const;

 private slots:
  void slotNewFileItems();
  void slotFileItemRemoved( K3bMovixFileItem* );
  void slotSubTitleItemRemoved( K3bMovixFileItem* );
  void slotDropped( KListView*, QDropEvent* e, QListViewItem* after );

 private:
  K3bMovixDoc* m_doc;

  QMap<K3bFileItem*, K3bMovixFileViewItem*> m_itemMap;
};

#endif
