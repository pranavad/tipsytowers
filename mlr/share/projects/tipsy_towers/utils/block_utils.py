import numpy as np

from scipy.optimize import linear_sum_assignment

from mlr.share.projects.tipsy_towers.utils.compute_utils import is_close
from mlr.share.projects.tipsy_towers.utils.constants import NameUtils


class BlockPosition:
    def __init__(self, x=-9, y=-9, z=-9):
        self._x = x
        self._y = y
        self._z = z

    def x(self):
        return self._x

    def y(self):
        return self._y

    def z(self):
        return self._z

    def get_values_as_list(self):
        return [self.x(), self.y(), self.z()]

    def __eq__(self, other):
        if isinstance(other, BlockPosition):
            condition_x = is_close(self.x(), other.x(), 2)
            condition_y = is_close(self.y(), other.y(), 2)
            condition_z = is_close(self.z(), other.z(), 2)
            if condition_x and condition_y and condition_z:
                return True
        return False


class BlockQuaternion:
    def __init__(self, x=-1, y=-1, z=-1, w=-1):
        self._x = x
        self._y = y
        self._z = z
        self._w = w

    def x(self):
        return self._x

    def y(self):
        return self._y

    def z(self):
        return self._z

    def w(self):
        return self._w

    def get_values_as_list(self):
        return [self.x(), self.y(), self.z(), self.w()]

    def __eq__(self, other):
        if isinstance(other, BlockQuaternion):
            condition_x = is_close(self.x(), other.x())
            condition_y = is_close(self.y(), other.y())
            condition_z = is_close(self.z(), other.z())
            condition_w = is_close(self.w(), other.w())
            if condition_x and condition_y and condition_z and condition_w:
                return True
        return False


class BlockShape:
    def __init__(self, block_width=0.05, block_length=0.1, block_height=0.05, block_edge_curvature=0.005):
        self.block_width = float(block_width)
        self.block_length = float(block_length)
        self.block_height = float(block_height)
        self.block_edge_curvature = float(block_edge_curvature)

    def get_block_width(self):
        return self.block_width

    def get_block_length(self):
        return self.block_length

    def get_block_height(self):
        return self.block_height


class BlockParams:
    def __init__(self,
                 position: BlockPosition,
                 rotation: BlockQuaternion,
                 shape: BlockShape,
                 block_underneath_name,
                 block_above_name,
                 supports_two_blocks):

        self._position = position
        self._rotation = rotation
        self._shape = shape
        self._block_underneath_name = block_underneath_name
        self._block_above_name = block_above_name
        self._supports_two_blocks = supports_two_blocks

    def get_position(self):
        return self._position

    def get_shape(self):
        return self._shape

    def get_rotation(self):
        return self._rotation

    def get_rotation_as_list(self):
        return self._rotation.get_values_as_list()

    def get_name_of_block_underneath(self):
        return self._block_underneath_name

    def get_name_of_block_above(self):
        return self._block_above_name

    def set_position(self, position):
        self._position = position

    def set_rotation(self, rotation):
        self._rotation = rotation

    def set_name_of_block_underneath(self, name_of_block_underneath):  # can be "table"
        self._block_underneath_name = name_of_block_underneath

    def set_name_of_block_above(self, is_below_name):
        self._block_above_name = is_below_name

    def supports_two_blocks(self):
        return self._supports_two_blocks

    def __eq__(self, other):
        if isinstance(other, BlockParams):
            condition2 = self.get_position() == other.get_position()
            condition3 = self.get_rotation() == other.get_rotation()
            condition1 = self.get_name_of_block_underneath() == other.get_name_of_block_underneath()
            condition4 = self.get_name_of_block_above() == other.get_name_of_block_above()
            if condition1 and condition2 and condition3 and condition4:
                return True
        return False
