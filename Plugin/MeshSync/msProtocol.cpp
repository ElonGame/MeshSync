#include "pch.h"
#include "msProtocol.h"
#include "msSceneGraphImpl.h"

namespace ms {

Message::~Message()
{
}
uint32_t Message::getSerializeSize() const
{
    return ssize(protocol_version);
}
void Message::serialize(std::ostream& os) const
{
    write(os, protocol_version);
}
void Message::deserialize(std::istream& is)
{
    read(is, protocol_version);
    if (protocol_version != msProtocolVersion) {
        throw std::exception("protocol version not matched");
    }
}

GetMessage::GetMessage()
{
}
uint32_t GetMessage::getSerializeSize() const
{
    uint32_t ret = super::getSerializeSize();
    ret += ssize(flags);
    ret += ssize(scene_settings);
    ret += ssize(refine_settings);
    return ret;
}
void GetMessage::serialize(std::ostream& os) const
{
    super::serialize(os);
    write(os, flags);
    write(os, scene_settings);
    write(os, refine_settings);
}
void GetMessage::deserialize(std::istream& is)
{
    super::deserialize(is);
    read(is, flags);
    read(is, scene_settings);
    read(is, refine_settings);
}


SetMessage::SetMessage()
{
}
uint32_t SetMessage::getSerializeSize() const
{
    uint32_t ret = super::getSerializeSize();
    ret += scene.getSerializeSize();
    return ret;
}
void SetMessage::serialize(std::ostream& os) const
{
    super::serialize(os);
    scene.serialize(os);
}
void SetMessage::deserialize(std::istream& is)
{
    super::deserialize(is);
    scene.deserialize(is);
}


uint32_t DeleteMessage::Identifier::getSerializeSize() const
{
    return ssize(path) + ssize(id);
}
void DeleteMessage::Identifier::serialize(std::ostream& os) const
{
    write(os, path); write(os, id);
}
void DeleteMessage::Identifier::deserialize(std::istream& is)
{
    read(is, path); read(is, id);
}

DeleteMessage::DeleteMessage()
{
}
uint32_t DeleteMessage::getSerializeSize() const
{
    uint32_t ret = super::getSerializeSize();
    ret += ssize(targets);
    return ret;
}
void DeleteMessage::serialize(std::ostream& os) const
{
    super::serialize(os);
    write(os, targets);
}
void DeleteMessage::deserialize(std::istream& is)
{
    super::deserialize(is);
    read(is, targets);
}


FenceMessage::~FenceMessage() {}
uint32_t FenceMessage::getSerializeSize() const
{
    return super::getSerializeSize()
        + ssize(type);
}
void FenceMessage::serialize(std::ostream& os) const
{
    super::serialize(os);
    write(os, type);
}
void FenceMessage::deserialize(std::istream& is)
{
    super::deserialize(is);
    read(is, type);
}

TextMessage::~TextMessage() {}
uint32_t TextMessage::getSerializeSize() const
{
    return super::getSerializeSize()
        + ssize(text)
        + ssize(type);
}
void TextMessage::serialize(std::ostream& os) const
{
    super::serialize(os);
    write(os, text);
    write(os, type);
}
void TextMessage::deserialize(std::istream& is)
{
    super::deserialize(is);
    read(is, text);
    read(is, type);
}


ScreenshotMessage::ScreenshotMessage() {}
uint32_t ScreenshotMessage::getSerializeSize() const { return super::getSerializeSize(); }
void ScreenshotMessage::serialize(std::ostream& os) const { super::serialize(os); }
void ScreenshotMessage::deserialize(std::istream& is) { super::deserialize(is); }


QueryMessage::QueryMessage()
{
}

uint32_t QueryMessage::getSerializeSize() const
{
    return super::getSerializeSize()
        + ssize(type);
}

void QueryMessage::serialize(std::ostream & os) const
{
    super::serialize(os);
    write(os, type);
}

void QueryMessage::deserialize(std::istream & is)
{
    super::deserialize(is);
    read(is, type);
}

ResponseMessage::ResponseMessage()
{
}

uint32_t ResponseMessage::getSerializeSize() const
{
    return super::getSerializeSize()
        + ssize(text);
}

void ResponseMessage::serialize(std::ostream & os) const
{
    super::serialize(os);
    write(os, text);
}

void ResponseMessage::deserialize(std::istream & is)
{
    super::deserialize(is);
    read(is, text);
}


PollMessage::PollMessage()
{}

uint32_t PollMessage::getSerializeSize() const
{
    return super::getSerializeSize()
        + ssize(type);
}

void PollMessage::serialize(std::ostream& os) const
{
    super::serialize(os);
    write(os, type);
}

void PollMessage::deserialize(std::istream& is)
{
    super::deserialize(is);
    read(is, type);
}


} // namespace ms
