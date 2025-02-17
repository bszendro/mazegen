
interface Size2D {
  width: number;
  height: number;
}

const PAPER_SIZES: { [id: string]: Size2D } = {
  a3: { width: 11.7, height: 16.5 },
  a4: { width: 8.3, height: 11.7 },
  a5: { width: 5.8, height: 8.3 },
};

const MARGIN_X = 0.39; // ~1cm
const MARGIN_Y = 0.39; // ~1cm
const PPI = 96;

/** Compute the rectangle available (in pixels) for drawing the maze based on the paper size */
export function getAreaSize(paperSize: string): Size2D {
  const size = PAPER_SIZES[paperSize];
  return {
    width: Math.floor((size.width - 2*MARGIN_X) * PPI),
    height: Math.floor((size.height - 2*MARGIN_Y) * PPI),
  };
}
