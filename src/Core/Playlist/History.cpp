#include "Core/Globals.h"
#include "Core/Log.h"

#include "History.h"

History::History(QObject *parent) : QObject(parent) {}

int History::size() const
{
    return m_entries.size();
}

void History::addEntry(const QPersistentModelIndex &entry)
{
    m_entries.push_back(entry);
    m_currentIndex++;
    Log_Debug("Adding entry to history. New size: " + QString::number(size()));
    adjustSize();
}

QPersistentModelIndex History::getEntry(int index) const
{
    if (index >= 0 && index < size())
        return m_entries[index];
    return {};
}

QPersistentModelIndex History::goEnd()
{
    m_currentIndex = size() - 1;
    return entry();
}

QPersistentModelIndex History::goStart()
{
    m_currentIndex = 0;
    return entry();
}

QPersistentModelIndex History::goForward()
{
    if(m_currentIndex + 1 > size() - 1) return {};
    m_currentIndex++;
    return entry();
}

QPersistentModelIndex History::goBack()
{
    if(m_currentIndex - 1 < 0) return {};
    m_currentIndex--;
    return entry();
}

QPersistentModelIndex History::entry()
{
    adjustSize();
    return m_entries[m_currentIndex];
}

void History::adjustSize()
{
    auto capacity = globals().historyCapacity() - 1;
    if(m_currentIndex > capacity) {
        int elementsToRemove = m_currentIndex - capacity;
        m_entries.erase(m_entries.begin(), m_entries.begin() + elementsToRemove);
        m_entries.shrink_to_fit();
        m_currentIndex -= elementsToRemove;
    }
}

void History::clear()
{
    m_entries.clear();
    m_currentIndex = 0;
}

void History::clearFuture()
{
    m_entries.erase(m_entries.begin() + m_currentIndex + 1, m_entries.end());
    Log_Debug("Deleting future history entries. New size: " + QString::number(size()));
}

bool History::isEmpty() const
{
    return m_entries.isEmpty();
}

bool History::hasForward() const
{
    return m_entries.size() > m_currentIndex + 1;
}

bool History::hasBack() const
{
    return m_currentIndex - 1 >= 0;
}

int History::currentIndex() const
{
    return m_currentIndex;
}
