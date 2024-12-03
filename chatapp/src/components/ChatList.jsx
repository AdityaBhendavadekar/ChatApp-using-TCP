import React from "react";

const ChatList = ({ chats, onChatSelect }) => {
  return (
    <div className="chat-list">
      {chats.map((chat) => (
        <div
          key={chat.id}
          className="chat-item"
          onClick={() => onChatSelect(chat)}
        >
          {chat.name}
        </div>
      ))}
    </div>
  );
};

export default ChatList;
