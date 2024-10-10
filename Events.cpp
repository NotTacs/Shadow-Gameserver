#include "Events.h"

void JerkyLoader::GetAllJerkyObjects() {
	std::ofstream stream("Jerky.log");

	for (int i = 0; i < UObject::GObjects->Num(); i++) {
		UObject* Object = UObject::GObjects->GetByIndex(i);

		if (!Object) continue;

		if (Object->GetFullName().contains("Jerky")) {
			stream << Object->GetFullName() << "\n";
		}
	}

	stream.close();
}