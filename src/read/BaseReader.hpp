#pragma once

#include "char.hpp"

template<typename Derived>
class BaseReader {
public:
	//Просто вернуть текущий символ
	const char_t readSymbol() inline const  {
		const Derived& self = static_cast<const Derived&>(*this);
		return self.readSymbolImpl();
	}
	//Подвинуться вперед или назад
	bool moveToSymbol(long long dif) {
		Derived& self = static_cast<Derived&>(*this);
		return self.moveToSymbolImpl(dif);
	}
	// БЕСПОЛЕЗНО Дать длину файла
	/*size_t size() const {
		const Derived& self = static_cast<const Derived&>(*this);
		return self.sizeImpl();
	}*/
	// ПОЛЕЗНО, НО ТРЕБУЕТ ПЕРЕРАБОТКУ ДЛЯ DOCX и PDF
	size_t getPos() const {
		const Derived& self = static_cast<const Derived&>(*this);
		return self.getPosImpl();
	}
	// ПОЛЕЗНО Проверить кончился ли файл
	bool empty() const {
		const Derived& self = static_cast<const Derived&>(*this);
		return self.emptyImpl();
	} 
	// БЕСПОЛЕЗНО, ЗАМЕНЕНО
	/*const char_t* getData() const {
		const Derived& self = static_cast<const Derived&>(*this);
		return self.getDataImpl();
	}*/
};