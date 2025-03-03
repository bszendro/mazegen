import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte'
import tailwindcss from '@tailwindcss/vite'
import { viteStaticCopy } from 'vite-plugin-static-copy'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    svelte(),
    tailwindcss(),
    viteStaticCopy({
      targets: [
        {
          src: 'MersenneTwister/mersenne-twister.js',
          dest: './'
        },
        {
          src: 'src/assets/qr-maze4kids-netli.svg',
          dest: './images/'
        }
      ],
    }),
  ],
})
