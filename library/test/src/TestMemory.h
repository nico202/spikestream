#ifndef TESTMEMORY_H

//Qt includes
#include <QThread>

namespace spikestream {

	class TestMemory : public QThread {
		Q_OBJECT

		public:
			void run();

		private slots:
			void runMemoryTests();
	};
}

#endif//TESTMEMORY_H


