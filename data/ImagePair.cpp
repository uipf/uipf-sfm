#include "ImagePair.hpp"

using namespace uipfsfm::data;


std::string ImagePair::getName() const {
	std::string nameA = getContent().first->getName();
	std::string nameB = getContent().second->getName();

	if (nameA.empty()) {
		return nameB;
	} else if(nameB.empty()) {
		return nameA;
	} else {
		return nameA + "-" + nameB;
	}
}