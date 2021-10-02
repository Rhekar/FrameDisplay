#ifndef MBTL_FRAMEDISPLAY_H
#define MBTL_FRAMEDISPLAY_H

#include <string>
#include <list>
#include <map>

#include "framedisplay.h"

#include "texture.h"

#include "mbtl_pack.h"

// ************************************************** mbtl_cg.cpp

struct MBTL_CG_Image;
struct MBTL_CG_Alignment;

class MBTL_CG {
protected:
	bool				m_loaded;

	char				*m_data;
	unsigned int			m_data_size;

	const unsigned int		*m_indices;

	unsigned int			m_nimages;

	const MBTL_CG_Alignment	*m_align;
	unsigned int			m_nalign;

	struct ImageCell {
		unsigned int		start;
		unsigned int		width;
		unsigned int		height;
		unsigned int		offset;
		unsigned short		type_id;
		unsigned short		bpp;
	};

	struct Image {
		ImageCell		cell[256];
	};

	Image				*m_image_table;
	unsigned int			m_image_count;

	void			copy_cells(
					const MBTL_CG_Image *image,
					const MBTL_CG_Alignment *align,
					unsigned char *pixels,
					unsigned int x1,
					unsigned int y1,
					unsigned int width,
					unsigned int height,
					unsigned int *palette,
					bool is_8bpp);

	void			build_image_table();

	const MBTL_CG_Image	*get_image(unsigned int n);
public:
	bool			load(MBTL_Pack *pack, const char *name);

	void			free();

	const char		*get_filename(unsigned int n);

	Texture			*draw_texture(unsigned int n,
					unsigned int *palette, bool to_pow2,
					bool draw_8bpp = 0);

	int			get_image_count();

				MBTL_CG();
				~MBTL_CG();
};

// ************************************************** mbtl_framedata.cpp

// cleverly organized to be similar to MBAC's frame data.

struct MBTL_Hitbox {
	short x1, y1, x2, y2;
};

struct MBTL_Frame_AF {
	// rendering data
	bool		active;

	int		frame;
	int		frame_unk;

	int		offset_y;
	int		offset_x;

	int		duration;
	int		AFF;

	int		blend_mode;

	unsigned char	alpha;
	unsigned char	red;
	unsigned char	green;
	unsigned char	blue;

	float		z_rotation;
	float		y_rotation;
	float		x_rotation;

	bool		has_zoom;
	float		zoom_x;
	float		zoom_y;

	int		AFJP;
};

struct MBTL_Frame_AS {
	// state data
	int		speed_flags;
	int		speed_horz;
	int		speed_vert;
	int		accel_horz;
	int		accel_vert;

	int		ASMV;

	int		stand_state;

	int		cancel_flags;
};

struct MBTL_Frame_AT {
	bool		active;

	int		guard_flags;

	int		proration;
	int		proration_type;

	int		damage;
	int		red_damage;
	int		dmg_unknown;
	int		circuit_gain;
};

struct MBTL_Frame_EF {
	int		command;
	int		parameter;
	int		values[12];
};

struct MBTL_Frame_IF {
	int		command;
	int		values[12];
};

struct MBTL_Frame {
	MBTL_Frame_AF	AF;

	MBTL_Frame_AS	*AS;
	MBTL_Frame_AT	*AT;

	MBTL_Frame_EF	*EF[8];
	MBTL_Frame_IF	*IF[8];

	MBTL_Hitbox	*hitboxes[33];
};

struct MBTL_Sequence {
	// sequence property data
	std::string	name;

	bool		is_move;
	std::string	move_name;
	int		move_meter;

	int		subframe_count;

	bool		initialized;

	char		*data;

	MBTL_Frame	*frames;
	unsigned int	nframes;

	MBTL_Hitbox	*hitboxes;
	unsigned int	nhitboxes;

	MBTL_Frame_AT	*AT;
	unsigned int	nAT;

	MBTL_Frame_AS	*AS;
	unsigned int	nAS;

	MBTL_Frame_EF	*EF;
	unsigned int	nEF;

	MBTL_Frame_IF	*IF;
	unsigned int	nIF;
};

class MBTL_FrameData {
private:
	MBTL_Sequence	**m_sequences;
	unsigned int	m_nsequences;

	bool		m_loaded;
public:
	bool		load(MBTL_Pack *pack, const char *filename);

	bool		load_move_list(MBTL_Pack *pack, const char *filename);

	int		get_sequence_count();

	MBTL_Sequence	*get_sequence(int n);

	void		free();

		MBTL_FrameData();
		~MBTL_FrameData();
};


// ************************************************** mbtl_character.cpp

class MBTL_Character {
protected:
	bool		m_loaded;

	MBTL_FrameData	m_framedata;

	char		*m_name;

	MBTL_CG	m_cg;

	unsigned int	**m_palettes;

	int		m_active_palette;

	Texture		*m_texture;
	int		m_last_sprite_id;

	bool		do_sprite_save(int id, const char *filename);

	MBTL_Frame *	get_frame(int seq_id, int fr_id);

	void		set_render_properties(const MBTL_Frame *frame, Texture *texture);
public:
	bool		load(MBTL_Pack *pack, const char *name, int sub_type);

	void		load_graphics(MBTL_Pack *pack);
	void		unload_graphics();

	void		render(const RenderProperties *properties, int seq, int frame);
	Clone		*make_clone(int seq_id, int fr_id);

	void		flush_texture();

	void		render_frame_properties(bool detailed, int scr_width, int scr_height, int seq, int frame);

	void		set_palette(int n);

	int		get_sequence_count();
	bool		has_sequence(int n);
	const char *	get_sequence_name(int n);
	const char *	get_sequence_move_name(int n, int *meter);

	int		get_frame_count(int seq_id);
	int		get_subframe_count(int seq_id);
	int		get_subframe_length(int seq_id, int fr_id);
	int		count_subframes(int seq_id, int fr_id);

	int		find_sequence(int seq_id, int direction);
	int		find_frame(int seq_id, int fr_id);

	const char *	get_current_sprite_filename(int seq_id, int fr_id);
	bool		save_current_sprite(const char *filename, int seq_id, int fr_id);
	int		save_all_character_sprites(const char *directory);

	void		free_frame_data();
	void		free_graphics();
	void		free();

			MBTL_Character();
			~MBTL_Character();
};

// ************************************************** mbtl_framedisplay.cpp

class MBTL_FrameDisplay : public FrameDisplay {
protected:
  MBTL_Pack	m_pack;

	MBTL_Character	m_character_data;

	int		m_subframe_base;
	int		m_subframe_next;
	int		m_subframe;

	void		set_active_character(int n);
	void		set_palette(int n);
	void		set_sequence(int n);
	void		set_frame(int n);
public:
	virtual const char *get_character_long_name(int n);
	virtual const char *get_character_name(int n);
	virtual int	get_sequence_count();
	virtual bool	has_sequence(int n);
	virtual const char *get_sequence_name(int n);
	virtual const char *get_sequence_move_name(int n, int *meter);
	virtual int	get_frame_count();
	virtual int	get_subframe();
	virtual int	get_subframe_count();

	virtual void	render(const RenderProperties *properties);
	virtual Clone	*make_clone();

	virtual void	flush_texture();

	virtual void	render_frame_properties(bool detailed, int scr_width, int scr_height);

	virtual void	command(FrameDisplayCommand command, void *param);

	virtual const char *get_current_sprite_filename();
	virtual bool	save_current_sprite(const char *filename);
	virtual int	save_all_character_sprites(const char *directory);

	virtual bool	init();
	virtual bool	init(const char *filename);

	virtual	void	free();

	MBTL_FrameDisplay();
	virtual		~MBTL_FrameDisplay();
};

#endif
