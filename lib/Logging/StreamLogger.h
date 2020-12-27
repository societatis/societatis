// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2020, The Qwertycoin Group.
// Copyright (c) 2020, Societatis.io
//
// This file is part of Societatis.
//
// Societatis is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Societatis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Societatis.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <mutex>
#include <Logging/CommonLogger.h>

namespace Logging {

class StreamLogger : public CommonLogger
{
public:
    explicit StreamLogger(Level level = DEBUGGING);
    explicit StreamLogger(std::ostream &stream, Level level = DEBUGGING);

    void attachToStream(std::ostream& stream);

protected:
    void doLogString(const std::string &message) override;

protected:
    std::ostream *m_stream;

private:
    std::mutex m_mutex;
};

} // namespace Logging
