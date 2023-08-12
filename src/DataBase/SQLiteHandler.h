#include <QObject>
#include <SQLiteCpp/SQLiteCpp.h>

namespace DB {
	struct SONG
	{
		SONG(std::string Title, std::string Artist, std::string Album, int length, int year, std::string filePath, int modifiedDate)
			: Title(Title), Artist(Artist), Album(Album), length(length), year(year), filePath(filePath), modifiedDate(modifiedDate)
		{}
		std::string Title;
		std::string Artist;
		std::string Album;
		int length;
		int year;
		std::string filePath;
		int modifiedDate;
	};

	class SQLiteHandler : public QObject
	{

		public:
			SQLiteHandler();
			void Open(const QString& dbPath, bool create_new = false);
			std::vector<DB::SONG> ReadSongs();
			void updateSongs(QString folderPath);
		private:
			SQLite::Database *db;
	};
}
