#ifndef WINDOWS_SERIAL_PORT_H_INCLUDED
#define WINDOWS_SERIAL_PORT_H_INCLUDED

void serial_port_configuration(void);
uint32_t read_serial_port(uint8_t *pBuffer, uint32_t len);
void close_serial_port(void);
void purge_serial_port(void);
void write_serial_port(uint8_t *dBuffer, uint32_t len);

#endif // WINDOWS_SERIAL_PORT_H_INCLUDED
