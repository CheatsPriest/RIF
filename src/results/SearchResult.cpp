#include <results/SearchResult.hpp>
#include <read/UnifiedReader.hpp>
#include <ICU/Decoders.hpp>

void SearchResult::loadContextFor(ConcretePlace& place)
{
	UnifiedReader reader(file_string_for_reader);
	auto u16context = reader.loadContext(place.left, place.right);
	place.context = utf16_to_utf8_icu(u16context);
}
