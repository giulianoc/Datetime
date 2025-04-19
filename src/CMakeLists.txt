
# Copyright (C) Giuliano Catrambone (giuliano.catrambone@catrasoftware.it)

# This program is free software; you can redistribute it and/or 
# modify it under the terms of the GNU General Public License 
# as published by the Free Software Foundation; either 
# version 2 of the License, or (at your option) any later 
# version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Commercial use other than under the terms of the GNU General Public
# License is allowed only after express negotiation of conditions
# with the authors.

SET (SOURCES
	Datetime.cpp
)

SET (HEADERS
	Datetime.h
)

#include_directories("${PROJECT_SOURCE_DIR}/../deploy/spdlog/include")

add_library (Datetime SHARED ${SOURCES} ${HEADERS})

target_compile_definitions(Datetime PRIVATE _REENTRANT)
target_compile_definitions(Datetime PRIVATE _FILE_OFFSET_BITS=64)

if(ZORAC)
	install (TARGETS Datetime DESTINATION services/cms-import)
	install (TARGETS Datetime DESTINATION services/cms-pusher)
	install (TARGETS Datetime DESTINATION services/cms-getter)
else()
	install (TARGETS Datetime DESTINATION lib)
	install (FILES ${HEADERS} DESTINATION include)
endif()

