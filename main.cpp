#include <iostream>
#include <string>

#include <SFML/Network.hpp>

#include "xwrapper.hpp"

#include "protocol.h"

#define PORT 2123

int main(int argc, char *argv[])
{
	XMouseInterface mouse;
	XKeyboardInterface keyboard;

	sf::SocketSelector my_selector;
	sf::TcpListener my_listener;
	std::vector<sf::TcpSocket*> my_sockets;

	std::cout << "listening" << std::endl;
	if(sf::Socket::Done != my_listener.listen(PORT))
	{
		std::cerr << "failed to listen at port: " << PORT << std::endl;
		return -1;
	}
	my_listener.setBlocking(false);
	my_selector.add(my_listener);
	for(;;)
	{
		if(my_selector.wait())
		{
			if(my_selector.isReady(my_listener))
			{
				sf::TcpSocket *new_socket = new sf::TcpSocket;
				sf::Socket::Status status;
				std::cout << "Someone connected!" << std::endl;
				status = my_listener.accept(*new_socket);
				if(sf::Socket::Done == status)
				{
					my_selector.add(*new_socket);
					my_sockets.push_back(new_socket);
				}
			}
			else
			{
				for(unsigned int i=0;i<my_sockets.size();i++)
				{
					if(my_selector.isReady(*my_sockets[i]))
					{
						std::vector<sf::TcpSocket*>::iterator it = std::find(my_sockets.begin(),my_sockets.end(),my_sockets[i]);
						sf::Socket::Status status;
						sf::Packet packet;
						status = (*it)->receive(packet);
						if(sf::Socket::Done == status)
						{
							unsigned int type = 0;
							packet >> type;
							if(KEYBOARD == type)
							{
								int state, keycode;
								packet >> state;
								packet >> keycode;
								if(PRESSED == state)
								{
									keyboard.PressKey(keycode);
								}
								else
								{
									keyboard.ReleaseKey(keycode);
								}
							}
							else if(KEYBOARD_MODIFIER == type)
							{
								int state, modifier, alt_lr;
								packet >> state;
								packet >> modifier;
								if(PRESSED == state)
								{
									switch(modifier)
									{
										case SHIFT:
											keyboard.PressShift();
											break;
										case ALT:
											packet >> alt_lr;
											if(LEFT == alt_lr) keyboard.PressAltL();
											else keyboard.PressAltR();
											break;
										case META:
											keyboard.PressMeta();
											break;
										case CTRL:
											keyboard.PressCtrl();
											break;
									}
								}
								else
								{
									switch(modifier)
									{
										case SHIFT:
											keyboard.ReleaseShift();
											break;
										case ALT:
											packet >> alt_lr;
											if(LEFT == alt_lr) keyboard.ReleaseAltL();
											else keyboard.ReleaseAltR();
											break;
										case META:
											keyboard.ReleaseMeta();
											break;
										case CTRL:
											keyboard.ReleaseCtrl();
											break;
									}
								}
							}
							else if(MOUSE == type)
							{
								unsigned int event;
								packet >> event;
								if(LEFT == event)
								{
									XMouseInterface::MouseState mstate;
									int state;
									packet >> state;
									if(0 == state)
									{
										mstate = XMouseInterface::BTN_DOWN;
									}
									else
									{
										mstate = XMouseInterface::BTN_UP;
									}
									mouse.MouseLeft(mstate);
								}
								else if(RIGHT == event)
								{
									XMouseInterface::MouseState mstate;
									int state;
									packet >> state;
									if(0 == state)
									{
										mstate = XMouseInterface::BTN_DOWN;
									}
									else
									{
										mstate = XMouseInterface::BTN_UP;
									}
									mouse.MouseRight(mstate);
								}
								else if(MOVE == event)
								{
									int x, y;
									packet >> x;
									packet >> y;
									mouse.MouseMove(x,y);
								}
							}
							else
							{
								std::cerr << "invalid message type" << std::endl;
							}
						}
						else
						{
							std::cout << "Someone disconnected!" << std::endl;
							(*it)->disconnect();
							my_sockets.erase(it);
						}
					}
				}
			}
		}
	}
	for(unsigned int i=0;i<my_sockets.size();i++)
	{
		my_selector.remove(*my_sockets[i]);
		my_sockets[i]->disconnect();
		delete my_sockets[i];
	}
	my_sockets.clear();
	my_listener.close();
	return 0;
}

