from ursina import *
from ursina.prefabs.first_person_controller import FirstPersonController
from ursina.shaders import lit_with_shadows_shader
from ursina.prefabs.health_bar import HealthBar
import random
import time

# Initialize the app
app = Ursina()
window.fullscreen = True
random.seed(0)
Entity.default_shader = lit_with_shadows_shader

# Lighting and Environment
sun = DirectionalLight()
sun.look_at(Vec3(1, -1, -1))
Sky()
ground = Entity(model='plane', texture='grass', scale=50, collider='box')  # Ground

# Boundary Walls
walls = [
    Entity(model='cube', collider='box', scale=(1, 5, 50), position=(25, 2.5, 0), color=color.gray),
    Entity(model='cube', collider='box', scale=(1, 5, 50), position=(-25, 2.5, 0), color=color.gray),
    Entity(model='cube', collider='box', scale=(50, 5, 1), position=(0, 2.5, 25), color=color.gray),
    Entity(model='cube', collider='box', scale=(50, 5, 1), position=(0, 2.5, -25), color=color.gray)
]

# Game Variables
game_running = False
score = 0
high_score = 0
time_left = 60
bullets = 20
max_bullets = 20
current_level = 1

# Player Setup
player = FirstPersonController()
player.enabled = False
player.max_hp = 100
player.hp = player.max_hp
player_health_bar = HealthBar(bar_color=color.lime.tint(-.25), roundness=0.5, parent=camera.ui, position=(-0.6, 0.45),
                              scale=(0.4, 0.04))

# Gun Setup
gun = Entity(model='cube', parent=camera, position=(.5, -.25, .25), scale=(.3, .2, 1), origin_z=-.5, color=color.red,
             on_cooldown=False)
gun.muzzle_flash = Entity(parent=gun, z=1, world_scale=.5, model='quad', color=color.yellow, enabled=False)

# UI Elements
main_menu = Entity(parent=camera.ui, enabled=True)
start_btn = Button(text="Start Game", scale=(0.3, 0.1), position=(0, 0.1), color=color.green, parent=main_menu)
exit_btn = Button(text="Exit", scale=(0.3, 0.1), position=(0, -0.1), color=color.red, parent=main_menu,
                  on_click=application.quit)

pause_btn = Button(text="Pause", scale=(0.15, 0.07), position=(0.52, 0.4), color=color.orange,
                   on_click=lambda: toggle_pause(), enabled=False)
restart_btn = Button(text="Restart", scale=(0.2, 0.1), position=(0.52, 0.2), color=color.azure,
                     on_click=lambda: restart_game(), enabled=False)
next_level_btn = Button(text="Next Level", scale=(0.3, 0.1), position=(0, 0), color=color.lime,
                        on_click=lambda: next_level(), enabled=False)

score_text = Text(text="Score: 0", position=(-0.7, 0.40), scale=1.5, color=color.white, background=True)
high_score_text = Text(text="High Score: 0", position=(-0.7, -0.38), scale=1.5, color=color.cyan, background=True)
timer_text = Text(text="Time: 60s", position=(0.45, -0.39), scale=1.5, color=color.yellow, background=True)
ammo_text = Text(text="Ammo: 10/10", position=(0.45, 0.30), scale=1.5, color=color.green, background=True)
level_complete_text = Text(text="Level Complete!", position=(0, 0.2), scale=2, color=color.green, background=True,
                           enabled=False)

# Enemies
shootables_parent = Entity()
enemies = []


class Enemy(Entity):
    def __init__(self, **kwargs):
        super().__init__(parent=shootables_parent, model='cube', scale_y=2, origin_y=-.5, color=color.light_gray,
                         collider='box', **kwargs)
        self.health_bar = Entity(parent=self, y=1.2, model='cube', color=color.red, world_scale=(1.5, .1, .1))
        self.max_hp = 100
        self.hp = self.max_hp
        self.last_attack = time.time()
        self.destroyed = False

    def update(self):
        if not game_running or self.destroyed:
            return

        dist = distance_xz(player.position, self.position)
        if dist > 40:
            return

        if dist > 2:
            self.look_at_2d(player.position, 'y')
            self.position += self.forward * time.dt * 2

        if dist <= 2 and time.time() - self.last_attack > 1:
            player.hp = max(0, player.hp - 10)
            player_health_bar.value = player.hp / player.max_hp
            self.last_attack = time.time()
            if player.hp <= 0:
                game_over()

    @property
    def hp(self):
        return self._hp

    @hp.setter
    def hp(self, value):
        self._hp = value
        if value <= 0 and not self.destroyed:
            self.destroyed = True
            destroy(self)
            check_level_complete()
            return
        self.health_bar.world_scale_x = self.hp / self.max_hp * 1.5
        self.health_bar.alpha = 1


def check_level_complete():
    global enemies
    if all(enemy.destroyed for enemy in enemies):
        game_running = False
        level_complete_text.enable()
        next_level_btn.enable()


def next_level():
    global current_level, time_left
    current_level += 1
    time_left = 60
    for enemy in enemies:
        destroy(enemy)
    enemies.clear()
    level_complete_text.disable()
    next_level_btn.disable()
    spawn_enemies()
    score_text.text = f"Level {current_level} - Score: {score}"
    invoke(update_timer, delay=1)


def start_game():
    global game_running, score, time_left, bullets, current_level
    print("Start button clicked!")  # Debug log to ensure the function is called
    game_running = True
    score = 0
    time_left = 60
    bullets = max_bullets
    current_level = 1
    player.hp = player.max_hp
    player_health_bar.value = 1  # Set health bar to full
    score_text.text = f"Level {current_level} - Score: {score}"
    timer_text.text = f"Time: {time_left}s"
    ammo_text.text = f"Ammo: {bullets}/{max_bullets}"
    main_menu.disable()  # Hide the main menu
    restart_btn.disable()
    pause_btn.enable()
    player.enabled = True
    mouse.locked = True
    spawn_enemies()
    invoke(update_timer, delay=1)


def restart_game():
    global game_running, score, time_left, bullets, current_level, enemies
    game_running = True
    score = 0
    time_left = 60
    bullets = max_bullets
    current_level = 1
    player.hp = player.max_hp
    player_health_bar.value = 1
    score_text.text = f"Level {current_level} - Score: {score}"
    timer_text.text = f"Time: {time_left}s"
    ammo_text.text = f"Ammo: {bullets}/{max_bullets}"
    level_complete_text.disable()
    next_level_btn.disable()
    pause_btn.text = "Pause"
    restart_btn.disable()
    mouse.locked = True

    for enemy in enemies:
        destroy(enemy)
    enemies.clear()
    spawn_enemies()

    player.enabled = True
    invoke(update_timer, delay=1)


def game_over():
    global game_running
    game_running = False
    restart_btn.enable()
    player.enabled = False
    mouse.locked = False


def spawn_enemies():
    global enemies
    for _ in range(current_level + 2):
        x = random.uniform(-20, 20)
        z = random.uniform(-20, 20)
        enemies.append(Enemy(position=(x, 1, z)))


def shoot():
    global bullets
    if not gun.on_cooldown and bullets > 0:
        gun.on_cooldown = True
        gun.muzzle_flash.enabled = True
        from ursina.prefabs.ursfx import ursfx
        ursfx([(0.0, 0.0), (0.1, 0.9), (0.15, 0.75), (0.3, 0.14), (0.6, 0.0)], volume=0.5, wave='noise',
              pitch=random.uniform(-13, -12), pitch_change=-12, speed=3.0)
        bullets -= 1
        ammo_text.text = f"Ammo: {bullets}/{max_bullets}"  # Update ammo count
        invoke(gun.muzzle_flash.disable, delay=.05)
        invoke(setattr, gun, 'on_cooldown', False, delay=.15)
        if mouse.hovered_entity and hasattr(mouse.hovered_entity, 'hp'):
            mouse.hovered_entity.hp -= 10
            mouse.hovered_entity.blink(color.red)
    elif bullets <= 0:
        print("Out of Ammo! Press 'R' to reload.")


def update_timer():
    global time_left, game_running, high_score
    if game_running and time_left > 0:
        time_left -= 1
        timer_text.text = f"Time: {time_left}s"
        invoke(update_timer, delay=1)
    elif time_left == 0:
        if score > high_score:
            high_score = score
            high_score_text.text = f"High Score: {high_score}"
        check_level_complete()


def input(key):
    global bullets
    if key == "left mouse down" and game_running:
        shoot()
    if key == "r" and bullets < max_bullets:
        reload_ammo()
    if key in ("escape", "p"):
        toggle_pause()


def reload_ammo():
    global bullets
    bullets = max_bullets
    ammo_text.text = f"Ammo: {bullets}/{max_bullets}"


def toggle_pause():
    global game_running
    application.paused = not application.paused
    mouse.locked = not application.paused
    pause_btn.text = "Resume" if application.paused else "Pause"
    game_running = not application.paused

    @property
    def hp(self):
        return self._hp

    @hp.setter
    def hp(self, value):
        self._hp = value
        if value <= 0 and not self.destroyed:
            self.destroyed = True
            global score
        score += 10  # Increment score by 10 for killing an enemy
        score_text.text = f"Score: {score}"  # Update the score UI
        destroy(self)
        check_level_complete()
        return

    self.health_bar.world_scale_x = self.hp / self.max_hp * 1.5
    self.health_bar.alpha = 1


def next_level():
    global current_level, time_left, score
    current_level += 1
    time_left = 60
    score += 50  # Add bonus points for leveling up
    score_text.text = f"Score: {score}"  # Update the score UI

    for enemy in enemies:
        destroy(enemy)
    enemies.clear()
    level_complete_text.disable()
    next_level_btn.disable()
    spawn_enemies()
    score_text.text = f"Level {current_level} - Score: {score}"
    invoke(update_timer, delay=1)


def game_over():
    global game_running, high_score
    game_running = False
    if score > high_score:
        high_score = score  # Update high score
        high_score_text.text = f"High Score: {high_score}"  # Update UI
    restart_btn.enable()
    player.enabled = False
    mouse.locked = False


def restart_game():
    global game_running, score, time_left, bullets, current_level, enemies
    game_running = True
    score = 0  # Reset score
    time_left = 60
    bullets = max_bullets
    current_level = 1
    player.hp = player.max_hp
    player_health_bar.value = 1
    score_text.text = f"Score: {score}"  # Reset score UI
    timer_text.text = f"Time: {time_left}s"
    ammo_text.text = f"Ammo: {bullets}/{max_bullets}"
    level_complete_text.disable()
    next_level_btn.disable()
    pause_btn.text = "Pause"
    restart_btn.disable()
    mouse.locked = True

    for enemy in enemies:
        destroy(enemy)
    enemies.clear()
    spawn_enemies()

    player.enabled = True
    invoke(update_timer, delay=1)


# Assign the start button's on_click property
start_btn.on_click = start_game

# Run the app
app.run()