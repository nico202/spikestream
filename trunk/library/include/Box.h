#ifndef BOX_H
#define BOX_H

namespace spikestream {

    class Box {
		public:
			Box();
			Box(float x1, float y1, float z1,float x2, float y2, float z2);
			Box(const Box& box);
			Box& operator=(const Box& rhs);
			~Box();
			void expand_percent(float percent);
			bool intersects(const Box& box);
			void translate(float dx, float dy, float dz);

			float x1;
			float y1;
			float z1;
			float x2;
			float y2;
			float z2;

		private:
			//========================  VARIABLES  =========================

    };

}

#endif//BOX_H

