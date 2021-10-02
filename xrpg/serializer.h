#pragma once

extern "C" int						strcmp(const char* s1, const char* s2) noexcept(true); // Compare two strings

// Abstract serializer
struct serializer {
	enum class kind { Text, Number, Array, Struct };
	struct node {
		const char*					name;
		node*						parent;
		kind						type;
		int							index;
		void*						object; // application defined data
		void*						metadata; // application defined metadata
		void*						requisit; // application defined metadata
		bool						skip; // set this if you want skip block
		//
		constexpr node(kind type = kind::Text) : parent(0), name(""), type(type), index(0), object(0), metadata(0), requisit(0), skip(false) {}
		constexpr node(node& parent, const char* name = "", kind type = kind::Text) : parent(&parent), name(name), type(type), index(0), object(0), metadata(0), requisit(0), skip(parent.skip) {}
		bool						operator==(const char* name) const { return name && strcmp(this->name, name) == 0; }
		//
		int							getlevel() const;
		bool						isparent(const char* id) const { return parent && *parent == id; }
	};
	struct reader {
		virtual void				error(const char* format, ...) {}
		virtual void				open(node& e) {}
		virtual void				set(node& e, const char* value) = 0;
		virtual void				close(node& e) {}
	};
	virtual ~serializer() {}
	virtual void					open(const char* id, kind type = kind::Text) = 0;
	virtual void					set(const char* id, int v, kind type = kind::Number) = 0;
	virtual void					set(const char* id, const char* v, kind type = kind::Text) = 0;
	virtual void					close(const char* id, kind type = kind::Text) = 0;
};
