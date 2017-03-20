#include <data/KeyPointList.hpp>
#include <sstream>


int main() {

	using namespace std;
	using namespace uipfsfm::data;

	KeyPointList::ptr keyPointList(new KeyPointList());

	// serialize
	ostringstream s1;
	keyPointList->serialize(s1);
	assert(s1.str() == string("0 128\n"));

	keyPointList->setContent(vector<cv::KeyPoint>());
	keyPointList->getContent().push_back(cv::KeyPoint(45, 54, 3.45, 90));
	cv::Mat* descriptor = new cv::Mat(1, 128, CV_8U);
	uint8_t d = 0;
	for(int v = 0; v < 128; ++v) {
		descriptor->at<uint8_t>(v) = d++;

	}
	keyPointList->descriptors = std::vector<cv::Mat*>();
	keyPointList->descriptors.push_back(descriptor);
	ostringstream s2;
	keyPointList->serialize(s2);
	//cout << s2.str() << endl;
	assert(s2.str() == string("1 128\n45 54 3.45 -1.5708\n 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19\n 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39\n 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59\n 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79\n 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99\n 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119\n 120 121 122 123 124 125 126 127\n"));


	// unserialize
	istringstream ss2(string("0 128\n"));
	KeyPointList::ptr keyPointList2(new KeyPointList(ss2));
	assert(keyPointList2->getContent().size() == 0);
	assert(keyPointList2->descriptors.size() == 0);

	istringstream ss3(string("1 128\n45 54 3.45 -1.5708\n 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19\n 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39\n 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59\n 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79\n 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99\n 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119\n 120 121 122 123 124 125 126 127\n"));
	KeyPointList::ptr keyPointList3(new KeyPointList(ss3));
	assert(keyPointList3->getContent().size() == 1);
	assert(keyPointList3->descriptors.size() == 1);
	cv::KeyPoint kp3 = keyPointList3->getContent()[0];
	assert(kp3.pt.x == 45);
	assert(kp3.pt.y == 54);
	assert(fabs(kp3.size - 3.45) < 0.0001);
	assert(fabs(kp3.angle - 90) < 0.0001);

	uint8_t dd = 0;
	for(int v = 0; v < 128; ++v) {
		cout << (int)keyPointList3->descriptors[0]->at<uint8_t>(v) << " - " << (int)dd << endl;
		assert(keyPointList3->descriptors[0]->at<uint8_t>(v) == dd);
		dd++;
	}

	istringstream ss4(string("2 0\n45 54 3.45 -1.5708\n46 64 5.43 0.785375\n"));
	KeyPointList::ptr keyPointList4(new KeyPointList(ss4));
	assert(keyPointList4->getContent().size() == 2);
	assert(keyPointList4->descriptors.size() == 0);

	cv::KeyPoint kp4a = keyPointList4->getContent()[0];
	assert(kp4a.pt.x == 45);
	assert(kp4a.pt.y == 54);
	assert(fabs(kp4a.size - 3.45) < 0.0001);
	assert(fabs(kp4a.angle - 90) < 0.0001);

	cv::KeyPoint kp4b = keyPointList4->getContent()[1];
	assert(kp4b.pt.x == 46);
	assert(kp4b.pt.y == 64);
	assert(fabs(kp4b.size - 5.43) < 0.0001);
	cout << kp4b.angle << endl;
	assert(fabs(kp4b.angle - 224) < 0.0001);


	return 0;
}